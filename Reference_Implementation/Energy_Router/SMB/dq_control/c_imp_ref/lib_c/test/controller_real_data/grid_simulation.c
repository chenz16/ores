#include "grid_simulation.h"
#include "./plant_simulator.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include "../../dq_to_modulation/dq_to_modulation.h"
#include "../../beta_transform/beta_transform_1p.h"
#include "../../log_data_rw/log_data_rw.h"
#include "../../lowpass_filter_1storder/lowpass_filter_1storder.h"
#include "../../misc/power_2dq_ref/power_2dq_ref.h"

#define SET_D_AXIS_AS_COS 0

void init_system_params(SystemParams* params) {
    params->signal_freq = 50.0f;
    params->plant_sim_freq = 1000.0f; //
    params->control_update_freq = 1000.0f; // 这是控制理想频率，跟sensing频率一致， 实际按照ratio_sens2control计算
    params->ratio_cntlFreqReduction = 1; // control 频率是 sensing 频率的 1/ratio_cntlFreqReduction
    params->Ts_plant_sim = 1.0f / params->plant_sim_freq;
    params->Ts_control = 1.0f / params->control_update_freq;
    params->omega = 2.0f * M_PI * params->signal_freq;
    params->Vg_rms = 230.0f;
    params->I_desired_rms =2.6f;
    params->R = 0.1f;
    params->L = 0.005f;
    params->sim_time = 0.4f;
    printf("Debug Ts values:\n");
    printf("Ts_plant_sim: %.6f\n", params->Ts_plant_sim);
    printf("Ts_control: %.6f\n", params->Ts_control);
    printf("omega: %.6f\n", params->omega);
    params->apparent_power = params->Vg_rms * params->I_desired_rms;  // S = V * I
    params->power_factor = -1.0f;  // Unity power factor by default
    params->reactive_power_is_leading = false;  // Default to lagging power factor
}

SimulationData* allocate_simulation_data(int length) {
    SimulationData* data = (SimulationData*)malloc(sizeof(SimulationData));
    data->length = length;
    
    // Allocate arrays
    data->t = (float*)calloc(length, sizeof(float));
    data->v_grid = (float*)calloc(length, sizeof(float));
    data->i_load = (float*)calloc(length, sizeof(float));
    data->v_inv_alpha = (float*)calloc(length, sizeof(float));
    data->v_inv_beta = (float*)calloc(length, sizeof(float));
    data->v_d_desired = (float*)calloc(length, sizeof(float));
    data->v_q_desired = (float*)calloc(length, sizeof(float));
    data->v_d_actual = (float*)calloc(length, sizeof(float));
    data->v_q_actual = (float*)calloc(length, sizeof(float));
    data->i_d_desired = (float*)calloc(length, sizeof(float));
    data->i_q_desired = (float*)calloc(length, sizeof(float));
    data->i_desired = (float*)calloc(length, sizeof(float));
    data->i_alpha_desired = (float*)calloc(length, sizeof(float));
    data->i_beta_desired = (float*)calloc(length, sizeof(float));

    data->i_alpha = (float*)calloc(length, sizeof(float));
    data->i_beta = (float*)calloc(length, sizeof(float));
    data->i_d = (float*)calloc(length, sizeof(float));
    data->i_q = (float*)calloc(length, sizeof(float));
    data->v_grid_alpha = (float*)calloc(length, sizeof(float));
    data->v_grid_beta = (float*)calloc(length, sizeof(float));
    data->v_grid_d = (float*)calloc(length, sizeof(float));
    data->v_grid_q = (float*)calloc(length, sizeof(float));
    // Add new allocations
    data->mod_index = (float*)calloc(length, sizeof(float));
    data->phase_shift = (float*)calloc(length, sizeof(float));
    data->mod_valid = (int*)calloc(length, sizeof(int));
    data->vdc = 400.0f;
    data->current_phase_shift = (float*)calloc(length, sizeof(float));

    
    return data;
}

void simulate_system(SystemParams* params, SimulationData* data) {
    // Change read_log_data to load_log_data
    LogData* log_data = load_log_data("../dq_decomp_real_data/log_data.csv");
    if (!log_data) {
        printf("Failed to read log data\n");
        return;
    }

    // Initialize controller with matching gains from Python
    DQController_Params controller_params = {
        .kp_d = 10.0f,        // Match Python kp value
        .ki_d = 200.0f,      // Match Python ki value
        .kp_q = 10.0f,
        .ki_q = 200.0f,
        .omega = params->omega,
        .Ts = params->Ts_control,
        .integral_max =  10.0f,   // Suggested value based on 230V RMS system
        .integral_min =  -10.0f,
        .R = params->R,      // Use system R value (0.1 ohm)
        .L = params->L       // Use system L value (0.005 H)
    };

    // Rest of initialization remains the same
    PlantParams plant_params = {
        .L = params->L,
        .R = params->R,
        .Ts = params->Ts_plant_sim,
        .omega = params->omega,
        .Vg_rms = params->Vg_rms,
        .Vg_mag = params->Vg_rms * sqrtf(2.0f),
        .Vg_phase = 0.0f
    };

    // Initialize all components
    PlantState plant_state = {0};
    BetaTransform_1p beta_transform_1p;
    DQController_State controller_state = {0};

    PlantSimulator_Init(&plant_state, &plant_params);
    BetaTransform_1p_Init(&beta_transform_1p,
                         params->signal_freq,
                         params->control_update_freq);
    DQController_Init(&controller_state, &controller_params);

    // Modify reference currents for single-phase system
    // For unity power factor (current in phase with voltage):
    //这里模拟，电流电压同步，功率因素为0 的情况；电流经过dq变换后， d = 0， q = -幅值
    // float i_ref_d = 20.0f;      // Changed from 20.0f to 0.0f
    // float i_ref_q = -0.0f;    // Changed from 0.0f to -20.0f
    // DQController_SetReference(&controller_state, i_ref_d, i_ref_q);
    // Grid voltage initialization
    float Vg_peak = params->Vg_rms * sqrtf(2.0f);
    float I_desired_peak = params->I_desired_rms * sqrtf(2.0f);
    float current_t = 0.0f;

    // Initialize low pass filters for Id and Iq
    LowPassFilter1st lpf_id = {0};
    LowPassFilter1st lpf_iq = {0};
    
    // Configure filters with appropriate cutoff frequency (e.g., 50 Hz)
    float cutoff_freq = 10.0f;  // Adjust this value as needed
    lpf_init(&lpf_id, params->control_update_freq, cutoff_freq);
    lpf_init(&lpf_iq, params->control_update_freq, cutoff_freq);

    // Main simulation loop
    dq_voltage_t dq_voltage_last = {0};
    int log_data_index = 0;  // Track position in log data

    /***
     * Loop for sensing and filter, 1k-20kHz
     ***/
    for (int n = 0; n < data->length - 1; n++) {
        float t = data->t[n];
        // float theta = params->omega * t;
        float theta = log_data->current_angle[n];
        float theta_dq = 0.0f;
        
        if (SET_D_AXIS_AS_COS) {
            // Grid voltage in alpha-beta
            data->v_grid[n] = Vg_peak * cosf(theta);
            data->v_grid_alpha[n] = data->v_grid[n];
            data->v_grid_beta[n] = Vg_peak * sinf(theta);
            theta_dq = theta;
        } else {
            // Grid voltage in alpha-beta
            data->v_grid[n] = Vg_peak * sinf(theta);
            data->v_grid_alpha[n] = data->v_grid[n];
            data->v_grid_beta[n] = -Vg_peak * cosf(theta);
            theta = theta - M_PI/2.0;
            theta_dq = theta;
        }

        // Transform grid voltage to dq
        dq_transform_1phase(data->v_grid_alpha[n], data->v_grid_beta[n], theta_dq,
                           &data->v_grid_d[n], &data->v_grid_q[n]);

        // Calculate current references using power-based approach
        float id_ref, iq_ref;
        power_to_dq_current_ref(
            params->apparent_power,
            params->power_factor,
            data->v_grid_d[n],
            data->v_grid_q[n],
            &id_ref,
            &iq_ref,
            params->reactive_power_is_leading
        );

        // Store the dq current references
        data->i_d_desired[n] = id_ref;
        data->i_q_desired[n] = iq_ref;

        // Transform back to alpha-beta for visualization/logging
        inverse_dq_transform_1phase(
            id_ref,
            iq_ref,
            theta_dq,
            &data->i_alpha_desired[n],
            &data->i_beta_desired[n]
        );

        // Replace simulated current with real current from log data
        if (log_data_index < log_data->length) {
            data->i_alpha[n] = log_data->current_value[log_data_index];
            data->i_beta[n] = BetaTransform_1p_Update(&beta_transform_1p, data->i_alpha[n]);
            log_data_index++;
        } else {
            printf("Warning: Ran out of log data at index %d\n", n);
            break;
        }

        // Transform to dq using adjusted angle
        // dq_transform_1phase(data->v_grid_alpha[n], data->v_grid_beta[n], theta_dq,
        //                    &data->v_grid_d[n], &data->v_grid_q[n]);

        // dq_transform_1phase(data->i_alpha_desired[n], data->i_beta_desired[n], theta_dq,
        //                    &data->i_d_desired[n], &data->i_q_desired[n]);
  
        dq_transform_1phase(data->i_alpha[n], data->i_beta[n], theta_dq,
                           &data->i_d[n], &data->i_q[n]);

        // Apply low pass filtering
        data->i_d[n] = lpf_process(&lpf_id, data->i_d[n]);
        data->i_q[n] = lpf_process(&lpf_iq, data->i_q[n]);
        data->current_phase_shift[n] = atan2f(data->i_q[n], data->i_d[n]);

        printf("v_grid_d: %.6f\n", data->v_grid_d[n]);
        printf("v_grid_q: %.6f\n", data->v_grid_q[n]);
        printf("i_d_desired: %.6f\n", data->i_d_desired[n]);
        printf("i_q_desired: %.6f\n", data->i_q_desired[n]);
        printf("i_d: %.6f\n", data->i_d[n]);
        printf("i_q: %.6f\n", data->i_q[n]);
        printf("current_phase_shift: %.6f\n", data->current_phase_shift[n]);
        printf("Goal d current is: %.6f,  + means discharging, - means charging\n", data->i_d_desired[n]);
        printf("Actual d current is: %.6f,  + means discharging, - means charging\n", data->i_d[n]);

        // Controller update

        ///控制器的跟新：输入为 电流dq， 输出为电压dq
        ///注意：为了计算法方便， 这里是跟sensing 的频率一致，实际按照控制频率
        DQController_UpdateMeasurements(&controller_state, 
                                      data->i_d[n], data->i_q[n],
                                      data->v_grid_d[n], data->v_grid_q[n]);
        DQController_SetReference(&controller_state, data->i_d_desired[n], data->i_q_desired[n]);

        DQController_Update(&controller_state, &controller_params);

        // Calculate modulation parameters
        dq_voltage_t dq_voltage = {
            .vd = DQController_GetVoltageD(&controller_state),  // Include grid feedforward
            .vq = DQController_GetVoltageQ(&controller_state),  // Include grid feedforward
            .vdc = data->vdc
        };
       
        ///调制参数跟新：输入为电压dq， 输出为调制系数和相位偏移
        modulation_result_t mod_result = dq_to_modulation_calculate(dq_voltage);
       /// 上层控制部分结束， 下面开始仿真 


       /// 调制参数更新
       dq_voltage.vd = mod_result.vd_adjust;
       dq_voltage.vq = mod_result.vq_adjust;
        /// /////////////////////////////////////////////////////////////
        /// 此处仿真逆变器电压波形生成，实际系统的 由PWM + 逆变器的物理反应为这部分
        if (n%params->ratio_cntlFreqReduction==0)
        {
          inverse_dq_transform_1phase(dq_voltage.vd,
                                    dq_voltage.vq,
                                    theta_dq,
                                    &data->v_inv_alpha[n],
                                    &data->v_inv_beta[n]);
        }
        else
        {
            inverse_dq_transform_1phase(dq_voltage_last.vd,
                dq_voltage_last.vq,
                theta_dq,
                &data->v_inv_alpha[n],
                &data->v_inv_beta[n]);
        }
        
        dq_voltage_last = dq_voltage;
       ///////////////////////////////////////////////////////////////////////


        ///此处仿真逆变器的电流波形生成，内部采用高频仿真，输出为低频
        // Store modulation results
        data->mod_index[n] = mod_result.index;
        data->phase_shift[n] = mod_result.phase_shift;
        data->mod_valid[n] = mod_result.valid;

        plant_params.Vg_phase = theta;
        plant_params.control_update_freq = params->control_update_freq;
        plant_params.plant_sim_freq = params->plant_sim_freq;
        // current_t = PlantSimulator_Update(&plant_state, &plant_params,
        //                                            data->v_inv_alpha[n],
        //                                            data->v_grid_alpha[n]);

        // data->i_load[n] = current_t;

        data->t[n + 1] = data->t[n] + params->Ts_control;
        data->i_load[n] =  data->i_alpha[n];
        ///////////////////// 模拟结束/////////////////////////////////////
    }

    // Use standard free instead of custom free function
    free(log_data->current_value);  // Free the array first
    free(log_data->time_s);         // Free the time array
    free(log_data);                 // Then free the structure itself
}

void free_simulation_data(SimulationData* data) {
    free(data->t);
    free(data->v_grid);
    free(data->i_load);
    free(data->v_inv_alpha);
    free(data->v_inv_beta);
    free(data->v_d_desired);
    free(data->v_q_desired);
    free(data->i_alpha);
    free(data->i_beta);
    free(data->i_d);
    free(data->i_q);
    free(data->v_grid_alpha);
    free(data->v_grid_beta);
    free(data->v_grid_d);
    free(data->v_grid_q);
    free(data->mod_index);
    free(data->phase_shift);
    free(data->mod_valid);
    free(data);
}

void save_results_to_file(const char* filename, SimulationData* data) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        printf("Error opening file %s\n", filename);
        return;
    }

    // Write header
    fprintf(fp, "t,v_grid,i_load,v_d_desired,v_q_desired,v_inv_alpha,v_inv_beta,"
            "i_alpha,i_beta,i_d,i_q,mod_index,phase_shift,mod_valid,current_phase_shift\n");

    // Write data with all values including modulation data
    for (int n = 0; n < data->length; n++) {
        fprintf(fp, "%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%d,%.6f\n",
                (double)data->t[n],
                (double)data->v_grid[n],
                (double)data->i_load[n],
                (double)data->v_d_desired[n],
                (double)data->v_q_desired[n],
                (double)data->v_inv_alpha[n],
                (double)data->v_inv_beta[n],
                (double)data->i_alpha[n],
                (double)data->i_beta[n],
                (double)data->i_d[n],
                (double)data->i_q[n],
                (double)data->mod_index[n],
                (double)data->phase_shift[n],
                (int)data->mod_valid[n],
                (double)data->current_phase_shift[n]);
    }

    fclose(fp);
    printf("Results saved to %s\n", filename);
}

