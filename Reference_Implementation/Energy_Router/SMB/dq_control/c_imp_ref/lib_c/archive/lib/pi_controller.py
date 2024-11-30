class PIController:
    def __init__(self, kp=1.0, ki=0.5, gain=1.0, 
                 integral_max=10.0, integral_min=-10.0, 
                 output_max=10.0, output_min=-10.0):
        """
        Initialize the PI Controller with the given parameters.

        Args:
            kp (float): Proportional gain.
            ki (float): Integral gain.
            gain (float): Scaling factor for the gains.
            integral_max (float): Maximum limit for the integral term.
            integral_min (float): Minimum limit for the integral term.
            output_max (float): Maximum output value.
            output_min (float): Minimum output value.
        """
        self.kp = gain * kp
        self.ki = gain * ki
        self.integral = 0.0
        self.integral_max = integral_max
        self.integral_min = integral_min
        self.output_max = output_max
        self.output_min = output_min

    def update(self, error, dt, reset_integral=False, integral_value=0.0):
        """
        Update the PI controller with the new error and compute the control signal.

        Args:
            error (float): The current error signal.
            dt (float): Time step.
            reset_integral (bool): Flag to reset the integral term.
            integral_value (float): Value to set the integral term if reset is True.

        Returns:
            float: The control signal after applying PI control.
        """
        if reset_integral:
            self.integral = integral_value
        else:
            # Update integral with clamping
            self.integral += error * dt
            self.integral = max(min(self.integral, self.integral_max), self.integral_min)

        # Compute control signal
        control_signal = self.kp * error + self.ki * self.integral

        # Clamp the control signal to output limits
        control_signal = max(min(control_signal, self.output_max), self.output_min)

        return control_signal