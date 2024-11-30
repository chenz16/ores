import numpy as np
from .notch_filter import ButterLowPassFilter
class VCO:
    def __init__(self, vco_freq_0=50.0, 
                 vco_freq_max=55.0, 
                 vco_freq_min=45.0, 
                 fs=1000, 
                 k0=1):
        """
        Initialize the VCO with parameters.

        Args:
            vco_freq_0 (float): Initial frequency of the VCO.
            vco_freq_max (float): Maximum frequency of the VCO.
            vco_freq_min (float): Minimum frequency of the VCO.
            fs (float): Sampling frequency.
            k0 (float): Gain factor for the control signal.
        """
        self.fs = fs
        self.k0 = k0
        self.vco_freq = vco_freq_0
        self.vco_theta = 0.0
        self.vco_freq_0 = vco_freq_0
        self.vco_freq_max = vco_freq_max
        self.vco_freq_min = vco_freq_min
        self.lpf = ButterLowPassFilter(400, fs)
    
    def update_phase(self, control_signal):
        self.vco_freq = self.vco_freq_0 + self.k0 * control_signal
        self.vco_freq = self.lpf.process(self.vco_freq)
        self.vco_freq = max(min(self.vco_freq, self.vco_freq_max), self.vco_freq_min)
        self.vco_theta += 2 * np.pi * self.vco_freq * (1.0 / self.fs)
        return self.vco_theta
    
    def get_updated_freq(self):
        return self.vco_freq

    def get_vco_output(self, output_type="angle"):
        """
        Get the current VCO output in the specified format.

        Args:
            output_type (str): The format of the output ("angle", "sin", "cos").

        Returns:
            float: The current VCO output in the specified format.
        """
        if output_type == "angle":
            return self.vco_theta
        elif output_type == "sin":
            return np.sin(self.vco_theta)
        elif output_type == "cos":
            return np.cos(self.vco_theta)
        else:
            raise ValueError("Invalid output_type. Choose from 'angle', 'sin', or 'cos'.")
        
