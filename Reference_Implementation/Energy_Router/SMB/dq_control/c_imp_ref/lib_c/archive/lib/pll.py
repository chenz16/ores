import numpy as np
from .phase_detector import PhaseDetector
from .vco import VCO
from .notch_filter import NotchFilterBank, ButterLowPassFilter
from .pi_controller import PIController
from .pll_data import PLLData
from scipy.signal import butter, lfilter, lfilter_zi
import sys


class PLL:
    def __init__(self, signal_freq=50.0, fs=1000, 
                 kd=1.0, kp=1.0, ki=1.0, k0 = 1.0, 
                 notch_filter_harmonics= range(2,7), 
                 signal_magnitude_est = 1.0, 
                 integral_max = 10.0, 
                 integral_min = -10.0, 
                 output_max = 10.0, 
                 output_min = -10.0, 
                 lpf_cutoff = 50.0):
        """
        Initialize the Phase-Locked Loop (PLL).

        Args:
            signal_freq (float): The base frequency of the input signal.
            fs (float): The sampling frequency.
            kd (float): The default value for Kd in the phase detector.
            notch_filter_harmonics (list): List of harmonics for the notch filter bank.
        """
        self.signal_freq = signal_freq
        self.fs = fs  
        self.phase_detector = PhaseDetector(kd = kd, 
                                            fs = fs, 
                                            est_input_signal_mag = signal_magnitude_est
        )
        self.vco = VCO(vco_freq_0 = signal_freq, 
                       vco_freq_max=signal_freq + 5, 
                       vco_freq_min=signal_freq - 5, 
                       fs=fs, 
                       k0 = k0)
        
        print("fs in pll: ", fs)
        
        self.notch_filter_bank = NotchFilterBank(notch_filter_harmonics, signal_freq, fs)
        self.lpf = ButterLowPassFilter(lpf_cutoff, fs)
        self.lpf2 = ButterLowPassFilter(5, fs)

        self.pi_controller = PIController(kp=kp, ki=ki, 
                                        gain=k0, 
                                        integral_max=integral_max, 
                                        integral_min=integral_min, 
                                        output_max=output_max, 
                                        output_min=output_min)
        self.curr_angle = 0.0
        self.next_angle = 0.0
        self.curr_freq = 0.0
        self.detected_phase_error = 0.0
        self.notched_phase_error = 0.0
        self.notched_phase_error_lpf  = 0.0
        self.curr_control_signal = 0.0

    def update_pll(self, signal_value, dynamic_kd_est=False, apply_lpf_b4_pi = False):
        """
        Update the PLL by processing the phase error and updating the VCO.

        Args:
            signal_value (float): The current value of the input signal.
            user_provided_kd (float): User-provided Kd value for phase detection.

        Returns:
            float: The current VCO phase (vco_theta).
        """
        # Detect phase error
        self.curr_angle = self.next_angle

        self.detected_phase_error  = self.phase_detector.detect_phase(
            signal_value,
            self.curr_angle,
            dynamic_kd_est=dynamic_kd_est
        )

        self.notched_phase_error = self.notch_filter_bank.process(self.detected_phase_error)


        if apply_lpf_b4_pi:
            self.notched_phase_error = self.lpf.process(self.notched_phase_error)

        self.notched_phase_error_lpf = self.lpf2.process(self.notched_phase_error)

        # self.notched_phase_error = 0.95*self.notched_phase_error_last + 0.05*self.notched_phase_error
        # self.notched_phase_error_last = self.notched_phase_error

        self.curr_control_signal = self.pi_controller.update(self.notched_phase_error, 
                                                             1.0/ self.fs)
        
        self.next_angle = self.vco.update_phase(self.curr_control_signal)
        self.curr_freq  = self.vco.get_updated_freq()

        return self.curr_angle
    
    def get_current_angle(self, output_type="angle"):
        return self.curr_angle
    
    def get_current_freq(self):
        return self.curr_freq
