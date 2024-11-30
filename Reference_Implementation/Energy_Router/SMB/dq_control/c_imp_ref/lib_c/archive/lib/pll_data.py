import numpy as np
import matplotlib.pyplot as plt

class PLLData:
    def __init__(self, length, phase_offset=0.0):
        """
        Initialize the PLL data storage.

        Args:
            length (int): The length of the time vector.
            phase_offset (float): The phase offset for compensation.
        """
        self.vco_freq_history = np.zeros(length)
        self.vco_theta_history = np.zeros(length)
        self.clean_phase_history = np.zeros(length)
        self.raw_phase_error_history = np.zeros(length)
        self.notch_filtered_history = np.zeros(length)
        self.real_phase_error = np.zeros(length)
        self.phase_offset = phase_offset
        self.notched_phase_error_lpf = np.zeros(length)

    def update(self, index, vco_freq, vco_theta, clean_phase, 
               phase_error, notch_filtered, real_phase_error, 
               notched_phase_error_lpf):
        """
        Update the PLL data at a specific index.

        Args:
            index (int): The current index in the time vector.
            vco_freq (float): The current VCO frequency.
            vco_theta (float): The current VCO phase.
            clean_phase (float): The clean phase of the signal.
            phase_error (float): The raw phase error.
            notch_filtered (float): The notch filtered error.
            real_phase_error (float): The real phase error.
        """
        self.vco_freq_history[index] = vco_freq
        self.vco_theta_history[index] = vco_theta
        self.clean_phase_history[index] = clean_phase
        self.raw_phase_error_history[index] = phase_error
        self.notch_filtered_history[index] = notch_filtered
        self.real_phase_error[index] = real_phase_error
        self.notched_phase_error_lpf[index] = notched_phase_error_lpf
    def plot_results(self, t, t_start, t_limit, signal_freq):
        """
        Plot the results of the PLL simulation.

        Args:
            t (np.ndarray): The time vector.
            t_start (int): The start index for plotting.
            t_limit (int): The end index for plotting.
            signal_freq (float): The frequency of the input signal.
        """
        sin_clean_phase_history = np.sin(self.clean_phase_history)
        sin_vco_theta_history = np.sin(self.vco_theta_history)
        sin_diff = [np.sin(self.vco_theta_history[i] - self.clean_phase_history[i]) for i in range(len(t))]

        plt.figure(figsize=(12, 21))  # Adjusted figure height
        total_sub_plot = 5
        # Plot original clean signal and VCO sine output
        plt.subplot(total_sub_plot, 1, 1)
        plt.plot(t[t_start:t_limit], sin_clean_phase_history[t_start:t_limit], label="Original Signal (Sine)", linestyle="solid")
        plt.plot(t[t_start:t_limit], sin_vco_theta_history[t_start:t_limit], label="Voltage Controlled Oscillator(VCO) Output", linestyle="dashed")
        plt.legend()

       # Plot difference between sin(theta) and sin(theta')
        plt.subplot(total_sub_plot, 1, 2)
        plt.plot(t[t_start:t_limit], sin_diff[t_start:t_limit], label="Difference between sin(signal_angle - detected_angle)", linestyle="solid")
        plt.legend()

        # Plot raw phase detection error
        plt.subplot(total_sub_plot, 1, 3)
        plt.plot(t[t_start:t_limit], np.zeros_like(t[t_start:t_limit]), label="Zero Line", linestyle="dashed")
        plt.plot(t[t_start:t_limit], self.real_phase_error[t_start:t_limit], label="Real Phase Error", linestyle="solid")
        plt.plot(t[t_start:t_limit], self.raw_phase_error_history[t_start:t_limit], label="Detected Phase Error", linestyle="dashed")
        plt.legend()

        # Plot phase error
        plt.subplot(total_sub_plot, 1, 4)
        plt.plot(t[t_start:t_limit], np.zeros_like(t[t_start:t_limit]), label="Zero Line", linestyle="dashed")
        plt.plot(t[t_start:t_limit], self.real_phase_error[t_start:t_limit], label="Real Phase Error", linestyle="solid")
        plt.plot(t[t_start:t_limit], 2.0 * self.notch_filtered_history[t_start:t_limit], label="Notched Phase Error")
        plt.plot(t[t_start:t_limit], 2.0 * self.notched_phase_error_lpf[t_start:t_limit], label="Notched Phase Error LPF")
        plt.legend()

        # # Plot VCO frequency
        # plt.subplot(total_sub_plot, 1, 5)
        # plt.plot(t[t_start:t_limit], self.vco_freq_history[t_start:t_limit], label="VCO Frequency")
        # plt.legend()

 
        # Plot original base frequency vs VCO frequency (before integration over time t)
        plt.subplot(total_sub_plot, 1, 5)
        plt.plot(t[t_start:t_limit], [signal_freq] * (t_limit - t_start), label="Original Base Frequency (50Hz)", linestyle="solid")
        plt.plot(t[t_start:t_limit], self.vco_freq_history[t_start:t_limit], label="VCO Frequency (before integration)", linestyle="dashed")
        plt.ylabel("Frequency (Hz)")
        plt.legend()

        plt.tight_layout()
        plt.show()