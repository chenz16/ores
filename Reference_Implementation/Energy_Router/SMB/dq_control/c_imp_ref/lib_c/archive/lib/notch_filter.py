import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import butter, lfilter, iirnotch, lfilter_zi

class RealTimeNotchFilter:
    def __init__(self, freq, fs, Q=10):
        """
        Initialize the real-time notch filter.
        
        Parameters:
        - freq: The frequency to be notched out.
        - fs: The sampling frequency.
        - Q: The quality factor of the notch filter.
        """
        w0 = freq / (fs / 2)  # Normalize the frequency
        self.b, self.a = iirnotch(w0, Q)
        self.zi = lfilter_zi(self.b, self.a)  # Initialize filter state

    def process(self, sample):
        """
        Process a single sample of data.
        
        Parameters:
        - sample: The input data sample to be filtered.
        
        Returns:
        - The filtered data sample.
        """
        sample = np.asarray([sample], dtype=float)
        filtered_sample, self.zi = lfilter(self.b, self.a, sample, zi=self.zi)
        return filtered_sample[0]
    
class NotchFilterBank:
    def __init__(self, harmonics, signal_freq, fs):
        """
        Initialize the Notch Filter Bank.

        Args:
            harmonics (list): List of harmonic multiples to create notch filters for.
            signal_freq (float): The base frequency of the input signal.
            fs (float): The sampling frequency.
        """
        self.filters = [RealTimeNotchFilter(h * signal_freq, fs) for h in harmonics]

    def process(self, signal_value):
        """
        Process the input signal through all notch filters.

        Args:
            signal_value (float): The current value of the input signal.

        Returns:
            float: The filtered signal value.
        """
        filtered_signal = signal_value
        for filter in self.filters:
            filtered_signal = filter.process(filtered_signal)
        return filtered_signal

class ButterLowPassFilter:
    def __init__(self, cutoff, fs, order=4, value_est = 1.0):
        self.b, self.a = butter(order, cutoff, btype='low', fs=fs)
        self.zi = lfilter_zi(self.b, self.a)
        self.zi = self.zi * value_est
    def process(self, sample):
        # Ensure sample is a 1-element array
        sample = np.asarray([sample], dtype=float)

        # Apply the filter with initial conditions
        filtered_sample, self.zi = lfilter(self.b, self.a, sample, zi=self.zi)
        return filtered_sample[0]  # Return the single filtered value

def main():
    # Test parameters matching the C test case
    fs = 1000.0  # Sampling frequency
    freq = 50.0  # Notch frequency
    Q = 10.0     # Quality factor
    
    # Create notch filter
    notch_filter = RealTimeNotchFilter(freq, fs, Q)
    
    # Print coefficients
    print("Notch Filter Coefficients:")
    print(f"b: {notch_filter.b}")
    print(f"a: {notch_filter.a}")
    
    # Calculate and print the normalized frequency
    w0 = freq / (fs / 2)
    print(f"\nNormalized frequency (w0): {w0}")
    
    # Generate test signal (same as C test)
    t = np.linspace(0, 0.1, 100)
    test_signal = np.sin(2 * np.pi * freq * t)
    
    # Process signal
    filtered_signal = np.array([notch_filter.process(sample) for sample in test_signal])
    
    # Plot results
    plt.figure(figsize=(12, 6))
    plt.plot(t, test_signal, label="Input Signal", color='blue')
    plt.plot(t, filtered_signal, label="Filtered Signal", color='red')
    plt.title(f"Notch Filter Response (f={freq}Hz, fs={fs}Hz, Q={Q})")
    plt.xlabel("Time [s]")
    plt.ylabel("Amplitude")
    plt.legend()
    plt.grid(True)
    plt.show()

if __name__ == "__main__":
    main()