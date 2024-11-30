import numpy as np
import matplotlib.pyplot as plt

class DataGenerator:
    def __init__(self, signal_freq=50.0,
                 harmonics=None, 
                 harmonic_amplitudes=None, 
                 phase_0=0.0,
                 gaussian_noise_level=0.0, 
                 fs=1000,
                 tmax=1.0, 
                 magnitude = 20.0):
        """
        Initialize the Data Generator.

        Args:
            signal_freq (float): The base frequency of the input signal.
            harmonics (list): List of harmonic multiples to include in the signal.
            harmonic_amplitudes (list): Amplitudes for each harmonic.
            phase_0 (float): Initial phase of the base signal.
            gaussian_noise_level (float): Standard deviation of Gaussian noise to add.
            fs (float): Sampling frequency.
            tmax (float): Maximum time for the signal duration.
        """
        self.signal_freq = signal_freq
        self.harmonics = harmonics if harmonics is not None else []
        self.harmonic_amplitudes = harmonic_amplitudes if harmonic_amplitudes is not None else []
        self.phase_0 = phase_0
        self.gaussian_noise_level = gaussian_noise_level
        self.fs = fs
        self.tmax = tmax
        self.t = np.arange(0, tmax, 1 / fs)
        self.magnitude = magnitude

    def generate(self):
        """
        Generate the sinusoidal signal with harmonics and noise.

        Returns:
            np.ndarray: The generated signal.
        """
        self.phase_base = 2 * np.pi * self.signal_freq * self.t + self.phase_0
        signal = self.magnitude * np.sin(self.phase_base)

        for i, h in enumerate(self.harmonics):
            signal += self.magnitude * self.harmonic_amplitudes[i] * np.sin(2 * np.pi * h * self.signal_freq * self.t + self.phase_0)

        # Add Gaussian noise
        noise = np.random.normal(0, self.gaussian_noise_level, len(self.t))
        signal += noise

        return signal

def main():
    # Create an instance of DataGenerator
    generator = DataGenerator(
        signal_freq=50.0,
        harmonics=[2, 3],
        harmonic_amplitudes=[0.5, 0.3],
        phase_0=0.0,
        gaussian_noise_level=0.1,
        fs=1000,
        tmax=1.0
    )
    
    # Generate the signal
    signal = generator.generate()
    
    # Plot the signal
    plt.figure(figsize=(10, 4))
    plt.plot(generator.t, signal)
    plt.title('Generated Signal')
    plt.xlabel('Time [s]')
    plt.ylabel('Amplitude')
    plt.grid(True)
    plt.show()

if __name__ == "__main__":
    main()