import numpy as np
from scipy.optimize import fsolve
import matplotlib.pyplot as plt
import struct
import random

def triangle_wave(angle, freq_ratio, triangle_init_phase):
    # Scale and phase shift the angle
    scaled_angle = (freq_ratio * angle + triangle_init_phase) % (2*np.pi)
    
    # Convert to range [0, 4] for easier triangle wave generation
    normalized = 2 * scaled_angle / np.pi
    
    # Generate triangle wave
    if normalized <= 1:  # Rising from -1 to 1
        return -1 + 2*normalized
    elif normalized <= 3:  # Falling from 1 to -1
        return 1 - (normalized - 1)
    else:  # Rising from -1 (completing the cycle)
        return -1 + (normalized - 3)
    
def find_intersection(m, ref_point, modulation_index):
    dx = 0
    for i in range(50):
        dx = modulation_index*np.sin(ref_point + dx)/m
    return dx

def generate_pwm_table(modulation_index, triangle_init_phase, freq_ratio=1):
    intersections = [0]  # Start point
    pwm_values = [0]    # Start with 0
    ref_points = [0]    # Start with 0
    dt_values = [0]  # Start with 0 for dt_left
    
    # First half: 0 to π
    for k in range(freq_ratio//2):
        # First half reference point
        ref_point = k * 2*np.pi/freq_ratio + triangle_init_phase
        
        m_left = - freq_ratio/np.pi  # Correct slope for rising edge
        m_right =  freq_ratio/np.pi  # Negative slope for falling edge
        
        # Get the dt values and add points for first half
        dt_left = find_intersection(m_left, ref_point, modulation_index)
        dt_right = find_intersection(m_right, ref_point, modulation_index)
        
        t_left = ref_point + dt_left
        t_right = ref_point + dt_right
        
        # Add points for first half (0 to π)
        intersections.append(t_left)
        pwm_values.append(1)  # rising edge = 0
        intersections.append(t_right)
        pwm_values.append(0)  # rising edge = 0

        ref_points.append(ref_point)  # Use actual ref_point
        ref_points.append(ref_point)  # Use actual ref_point

        dt_values.append(dt_left)
        dt_values.append(dt_right)

    right_intersections = []  # Start point
    right_pwm_values = []    # Start with 0
    right_ref_points = []    # Start with 0
    right_dt_values = []  # Start with 0 for dt_left
     

    for i in range(freq_ratio):
        right_intersections.append(2.0*np.pi - intersections[freq_ratio - i])
        right_pwm_values.append(pwm_values[freq_ratio - i]-1)
        right_ref_points.append(ref_points[freq_ratio - i])
        right_dt_values.append(dt_values[freq_ratio - i])
        print('i===', i)

    print('right_intersections===', len(right_intersections))
    print('intersections===', len(intersections))

    intersections = intersections + right_intersections
    pwm_values = pwm_values + right_pwm_values
    ref_points = ref_points + right_ref_points
    dt_values = dt_values + right_dt_values

    print('intersections2==', len(intersections))

    intersections.append(2*np.pi) 
    pwm_values.append(0)
    ref_points.append(2*np.pi)
    dt_values.append(0)
 
    return intersections, pwm_values, ref_points, dt_values

def save_tables_to_binary(filename, tables, dimensions):
    with open(filename, 'wb') as f:
        # Write dimensions first (N_init_phase, N_mod_index, points_per_cycle)
        for dim in dimensions:
            f.write(struct.pack('I', dim))
        
        # Write all table data as float32
        for table in tables:
            for phase_data in table:
                for mod_data in phase_data:
                    for value in mod_data:
                        f.write(struct.pack('f', float(value)))

if __name__ == "__main__":
    mod_index = 0.8
    freq_ratio = 20
    min_phase = 2.0*2.0*np.pi/freq_ratio/100
    init_phase = 1*2.0*np.pi/freq_ratio/4 + min_phase

    init_phsae_max = 2.0*np.pi/freq_ratio;
    N_init_phase = 10
    init_phase_list = np.linspace(0.0, init_phsae_max, N_init_phase)
    N_mod_index = 10
    mod_index_list = np.linspace(0.5, 1.0, N_mod_index)

    # Initialize empty lists for the lookup tables
    intersection_table = []  # [phase][mod][index]
    pwm_value_table = []    # [phase][mod][index]
    ref_point_table = []    # [phase][mod][index]
    dt_value_table = []     # [phase][mod][index]
    
    for i, init_phase in enumerate(init_phase_list):
        intersection_table.append([])
        pwm_value_table.append([])
        ref_point_table.append([])
        dt_value_table.append([])
        
        for j, mod_index in enumerate(mod_index_list):
            intersections, pwm_values, ref_points, dt_values = generate_pwm_table(mod_index, init_phase, freq_ratio)
            
            # Store the values in the lookup tables
            intersection_table[i].append(intersections)
            pwm_value_table[i].append(pwm_values)
            ref_point_table[i].append(ref_points)
            dt_value_table[i].append(dt_values)
    
    # Now you can access values using:
    # intersection_table[phase_index][mod_index][intersection_index]
    # pwm_value_table[phase_index][mod_index][intersection_index]
    # etc.

    # Optional: Print table dimensions
    print(f'Table dimensions:')
    print(f'Phase variations: {len(init_phase_list)}')
    print(f'Modulation variations: {len(mod_index_list)}')
    print(f'Intersection points: {len(intersection_table[0][0])}')  # 2*freq_ratio + 1

    # Randomly select indices
    random_phase_idx = random.randint(0, len(init_phase_list) - 1)
    random_mod_idx = random.randint(0, len(mod_index_list) - 1)
    
    # Get the actual values for these indices
    selected_phase = init_phase_list[random_phase_idx]
    selected_mod = mod_index_list[random_mod_idx]
    
    # Get the corresponding data
    intersections = intersection_table[random_phase_idx][random_mod_idx]
    pwm_values = pwm_value_table[random_phase_idx][random_mod_idx]
    
    # Create the plot
    plt.figure(figsize=(12, 6))
    
    # Plot sine wave
    x = np.linspace(0, 2*np.pi, 1000)
    y = selected_mod * np.sin(x)
    plt.plot(x, y, 'b-', label='Sine Wave')
    
    # Plot PWM waveform
    for i in range(len(intersections)-1):
        plt.plot([intersections[i], intersections[i+1]], 
                [pwm_values[i], pwm_values[i]], 
                'r-', linewidth=2)
        if i < len(intersections)-1:
            plt.plot([intersections[i+1], intersections[i+1]], 
                    [pwm_values[i], pwm_values[i+1]], 
                    'r-', linewidth=2)
    
    plt.grid(True)
    plt.xlabel('Angle (radians)')
    plt.ylabel('Amplitude')
    plt.title(f'Sine Wave and PWM\nPhase={selected_phase:.3f}, Modulation Index={selected_mod:.3f}')
    plt.legend()
    plt.show()

    # Package dimensions
    dimensions = [
        len(init_phase_list),      # N_init_phase
        len(mod_index_list),       # N_mod_index
        len(intersection_table[0][0])  # points_per_cycle
    ]

    # Save all tables as before
    save_tables_to_binary('pwm_tables.bin', 
                         [intersection_table, pwm_value_table, 
                          ref_point_table, dt_value_table],
                         dimensions)

    # Now add the single PWM table generation and saving
    print("\nGenerating single PWM table...")
    mod_index = 0.8
    init_phase = 0.0
    
    # Generate single PWM table
    _, single_pwm_table, _, _ = generate_pwm_table(mod_index, init_phase, freq_ratio)
    
    # Print the values
    print("\nSingle PWM Table Values (mod_index=0.8, init_phase=0.0):")
    for i, value in enumerate(single_pwm_table):
        print(f"Index {i}: {value}")
    print(f'Total number of points: {len(single_pwm_table)}')
    
    # Save single PWM table
    with open('single_pwm_table.bin', 'wb') as f:
        f.write(struct.pack('I', len(single_pwm_table)))
        for value in single_pwm_table:
            f.write(struct.pack('f', float(value)))
