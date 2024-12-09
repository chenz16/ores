import casadi as ca
import numpy as np

def optimize_switching_angles(Vi, V1_desired=311):
    """
    Optimize switching angles for an N-level MMC using CasADi.
    Args:
        Vi: List of voltage steps [V]
        V1_desired: Desired peak AC output voltage (default 311V)
    Returns:
        alpha: Optimized switching angles
    """
    num_angles = len(Vi)
    
    try:
        # Create optimization variables
        alpha = ca.SX.sym('alpha', num_angles)
        
        print(f"\nDC Total Voltage: {Vi[-1]:.2f}V")
        print(f"Desired V1: {V1_desired:.2f}V")
        
        # Fundamental component (k=1)
        V1 = (4 / ca.pi) * ca.sum1(Vi * ca.sin(alpha))
        
        # Specific harmonics to minimize
        harmonics = []
        for k in [3, 5, 7, 9, 11, 13, 15]:
            Vk = (4 / (k * ca.pi)) * ca.sum1(Vi * ca.sin(k * alpha))
            harmonics.append(Vk/k)
        
        # Objective function
        obj = 0
        # Fundamental tracking
        obj += ca.sumsqr((V1 - V1_desired)/V1_desired)
        
        # Minimize harmonics
        for i, Vh in enumerate(harmonics):
            k = 2*i + 3  # harmonic order
            obj += ca.sumsqr(Vh/V1_desired)
        
        # Constraints
        g = []
        lbg = []
        ubg = []
        
        # Fundamental component constraint
        g.append(V1)
        lbg.append(V1_desired - 0.005 * V1_desired)
        ubg.append(V1_desired + 0.005 * V1_desired)
        
        # Harmonic constraints with relaxed bounds
        for Vh in harmonics:
            g.append(Vh)
            lbg.append(-0.000001 * V1_desired)  # Allow ±5% of V1_desired
            ubg.append(0.000001 * V1_desired)
        
        # Angle ordering constraints
        for i in range(num_angles-1):
            g.append(alpha[i+1] - alpha[i])
            lbg.append(0.035)  # 2 degrees in radians
            ubg.append(0.262)  # 15 degrees in radians
        
        # Variable bounds
        lbx = [0] * num_angles
        ubx = [ca.pi/2] * num_angles
        
        # Create solver
        nlp = {'x': alpha, 'f': obj, 'g': ca.vertcat(*g)}
        opts = {
            'ipopt.max_iter': 1000,
            'ipopt.tol': 1e-6,
            'ipopt.acceptable_tol': 1e-4,
            'ipopt.print_level': 0,
            'print_time': 0
        }
        solver = ca.nlpsol('solver', 'ipopt', nlp, opts)
        
        # Initial guess - spread angles using CasADi operations
        x0 = [(i * ca.pi/3 + ca.pi/12)/num_angles for i in range(num_angles)]
        
        # Solve
        sol = solver(x0=x0, lbx=lbx, ubx=ubx, lbg=lbg, ubg=ubg)
        
        # Get solution
        alpha_opt = np.array(sol['x']).flatten()
        
        # Convert to switching angles
        theta = ca.pi/2 - alpha_opt
        
        return theta
        
    except Exception as e:
        print(f"Error during optimization: {str(e)}")
        return np.linspace(0.175, 1.222, num_angles)  # 10 to 70 degrees in radians

# Modify the main block to include testing of the real-time method
if __name__ == "__main__":
    # Generate random voltages around 50V with ±5V variation
    mean_voltage = 50.0
    voltage_variation = 5.0
    V_modules = np.random.uniform(
        mean_voltage - voltage_variation,
        mean_voltage + voltage_variation,
        8
    )
    V_modules = np.round(V_modules, 1)
    V_modules.sort()
    
    # Calculate voltage steps (Vi) - each step is individual module voltage
    Vi = V_modules.copy()  # Use module voltages directly
    
    print("\n=== DC Module Voltages ===")
    for i, v in enumerate(V_modules):
        print(f"V{i+1}: {v:.1f}V")
    print(f"Total DC Voltage: {sum(V_modules):.1f}V")
    print(f"Average DC Voltage: {np.mean(V_modules):.1f}V")
    
    V1_desired = 311  # Define desired fundamental voltage
    
    print("\n=== CasADi Optimization ===")
    angles_casadi = optimize_switching_angles(Vi, V1_desired)
    
    def calculate_harmonics(angles, voltages):
        # Calculate fundamental (k=1)
        V1 = abs((4 / np.pi) * sum(v * np.cos(a) for v, a in zip(voltages, angles)))
        
        # Calculate higher harmonics
        harmonics = []
        for k in [3, 5, 7, 9, 11, 13, 15]:
            Vk = abs((4 / (k * np.pi)) * sum(v * np.cos(k * a) 
                 for v, a in zip(voltages, angles)))
            harmonics.append(Vk)
        return V1, harmonics
    
    # Calculate harmonics
    V1_casadi, h_casadi = calculate_harmonics(angles_casadi, V_modules)
    
    print("\n=== Switching Angles (degrees) ===")
    print("Module    Voltage    Angle")
    print("---------------------------")
    for i, (v, ac) in enumerate(zip(V_modules, np.rad2deg(angles_casadi))):
        print(f"M{i+1:d}       {v:4.1f}V    {ac:6.2f}")
    
    print(f"\n=== Fundamental Component ===")
    print(f"Desired:   {V1_desired:7.2f}V")
    print(f"Actual:    {V1_casadi:7.2f}V  (Error: {100*(V1_casadi-V1_desired)/V1_desired:6.2f}%)")
    
    print("\n=== Harmonics (V) ===")
    print("Order     Value")
    print("----------------")
    for k, hc in enumerate(h_casadi):
        harmonic = 2*k + 3
        print(f"{harmonic:3d}      {hc:7.2f}")
    
    print("\n=== THD ===")
    def calculate_thd(harmonics, V1):
        return 100 * np.sqrt(sum(h*h for h in harmonics)) / V1
    
    print(f"THD:    {calculate_thd(h_casadi, V1_casadi):5.2f}%")