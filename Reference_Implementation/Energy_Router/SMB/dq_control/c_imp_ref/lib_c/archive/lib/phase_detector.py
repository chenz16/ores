import numpy as np
from scipy.signal import butter, lfilter_zi, lfilter
from lib.notch_filter import ButterLowPassFilter

class PhaseDetector:
    def __init__(self, kd=1.0, fs=1000.0, est_input_signal_mag=1.0):
        """
        Initialize the Phase Detector.

        Args:
            kd (float): The default value for Kd.
        """
        self.Kd = kd
        self.phase_error = 0
        self.fs = fs
        self.signal_value_last = 0
        self.lpf_mag = ButterLowPassFilter(10.0, fs, value_est = est_input_signal_mag)

    def dynamic_kd_est(self, signal_value, nominal_signal_freq = 50.0):
        single_stp_phase = 2.0 * np.pi * nominal_signal_freq / self.fs
        sin_half = (signal_value + self.signal_value_last) / np.cos(single_stp_phase / 2.0) / 2.0
        cos_half = (signal_value - self.signal_value_last) / np.sin(single_stp_phase / 2.0) / 2.0
        mag = np.sqrt(sin_half**2 + cos_half**2)
        mag = self.lpf_mag.process(mag)
        print(f"mag after filter: {mag}")
        self.signal_value_last = signal_value
        return mag

    def detect_phase(self, signal_value, vco_theta, dynamic_kd_est=False):
        """
        Perform phase detection.

        Args:
            signal_value (float): The current value of the input signal.
            vco_theta (float): The current VCO phase.
            dynamic_kd_est (bool): Whether to dynamically estimate Kd.

        Returns:
            float: The phase error.
        """
        mag = self.dynamic_kd_est(signal_value)

        if dynamic_kd_est:
            self.Kd = 1.0 / mag
            print(f"mag: {mag}")

        self.phase_error = signal_value * np.cos(vco_theta)
        self.phase_error *= self.Kd
        return self.phase_error