import casadi as ca
import numpy as np

def calculate_switching_angles_4modules(ci, m):
    """
    Calculate switching angles for 4 modules using CasADi optimization.
    Args:
        ci: Array of 4 normalized voltage coefficients
        m: Modulation index
    Returns:
        theta: Array of 4 switching angles in radians
    """
    num_angles = 4
    V_avg = 1.0
    Vi = ci * V_avg
    V1_desired = m * np.pi
    
    try:
        theta = ca.SX.sym('theta', num_angles)
        
        # Fundamental component
        V1 = ca.sum1(Vi * ca.cos(theta))
        
        # Harmonics with proper scaling
        harmonics = []
        for k in [3, 5, 7]:  # Only calculate up to 7th for constraints
            Vk = ca.sum1(Vi * ca.cos(k * theta)) / k
            harmonics.append(Vk)
        
        # Calculate higher harmonics for objective function
        higher_harmonics = []
        for k in [9, 11, 13, 15]:
            Vk = ca.sum1(Vi * ca.cos(k * theta)) / k
            higher_harmonics.append(Vk)
        
        # Objective function
        obj = 0
        # Fundamental tracking with higher weight
        obj += 100 * ca.sumsqr((V1 - V1_desired)/V1_desired)
        
        # Minimize higher harmonics
        for Vh in higher_harmonics:
            obj += 10 * ca.sumsqr(Vh/V1_desired)
        
        g = []
        lbg = []
        ubg = []
        
        # Fundamental constraint with tighter tolerance
        g.append(V1)
        tol = 0.0025 * V1_desired  # 0.25% tolerance
        lbg.append(V1_desired - tol)
        ubg.append(V1_desired + tol)
        
        # Hard constraints for 3rd, 5th, and 7th harmonics only
        for Vh in harmonics:
            g.append(Vh)
            max_h = 0.001 * V1_desired  # 0.1% of fundamental maximum
            lbg.append(-max_h)
            ubg.append(max_h)
        
        # Angle ordering constraints
        for i in range(num_angles-1):
            g.append(theta[i+1] - theta[i])
            lbg.append(0.00)  # Minimum gap
            ubg.append(0.5)   # Maximum gap (~28.6 degrees)
        
        # Variable bounds
        lbx = [0] * num_angles
        ubx = [ca.pi/2] * num_angles  # 90 degrees max
        
        nlp = {'x': theta, 'f': obj, 'g': ca.vertcat(*g)}
        opts = {
            'ipopt.max_iter': 1000,
            'ipopt.tol': 1e-6,
            'ipopt.acceptable_tol': 1e-4,
            'ipopt.print_level': 0,
            'print_time': 0,
            'ipopt.hessian_approximation': 'exact'
        }
        solver = ca.nlpsol('solver', 'ipopt', nlp, opts)
        
        # Initial guess - spread angles
        x0 = [(i * ca.pi/3 + ca.pi/12)/num_angles for i in range(num_angles)]
        
        sol = solver(x0=x0, lbx=lbx, ubx=ubx, lbg=lbg, ubg=ubg)
        theta_opt = np.array(sol['x']).flatten()
        
        return theta_opt
        
    except Exception as e:
        print(f"Error during optimization: {str(e)}")
        return np.linspace(0.175, 1.222, num_angles)

if __name__ == "__main__":
    # System parameters with random ci
    num_modules = 4
    m_test = 0.8  # Fixed modulation index
    
    # Generate random normalized coefficients between 0.975 and 1.025
    ci_test = np.random.uniform(0.975, 1.025, num_modules)
    ci_test = np.sort(ci_test)[::-1]  # Sort coefficients in descending order
    
    print("=== System Parameters ===")
    print(f"Number of modules: {num_modules}")
    print(f"Modulation index (m): {m_test:.6f}")
    print("\nNormalized voltage coefficients (ci):")
    for i, ci in enumerate(ci_test):
        print(f"ci[{i+1}]: {ci:.6f}")
    print(f"m*π = {m_test * np.pi:.6f}")
    print(f"Required average cos(θ) = {(m_test * np.pi)/4:.6f}")
    print(f"This implies θ ≈ {np.arccos((m_test * np.pi)/4):.6f} rad = {np.degrees(np.arccos((m_test * np.pi)/4)):.2f}°")
    
    angles = calculate_switching_angles_4modules(ci_test, m_test)
    
    print("\n=== Results ===")
    print("\nSwitching angles (rad):")
    total_cos = 0
    for i, angle in enumerate(angles):
        cos_val = np.cos(angle)
        total_cos += cos_val
        print(f"θ{i+1}: {angle:.6f} rad = {np.degrees(angle):.2f}°")
        print(f"cos(θ{i+1}): {cos_val:.6f}")
    
    print(f"\nSum of cos(θi): {total_cos:.6f}")
    
    V1 = float(sum(ci_test * np.cos(angles)))
    V1_desired = m_test * np.pi
    
    print(f"\nFundamental:")
    print(f"sum(ci*cos(θi)) = {V1:.6f}")
    print(f"Desired (m*π) = {V1_desired:.6f}")
    print(f"Error: {100*(V1-V1_desired)/V1_desired:.4f}%")
    
    print("\nHarmonics:")
    harmonics = []
    for k in [3, 5, 7, 9, 11, 13, 15, 17, 19]:  # Extended harmonic list
        Vk = float(sum(ci_test * np.cos(k * angles)) / k)
        harmonics.append(abs(Vk))
        print(f"H{k}: {abs(Vk):.6f} ({100*abs(Vk)/abs(V1):.4f}% of fundamental)")
    
    print("\n=== THD Calculation ===")
    thd = 100 * np.sqrt(sum(h*h for h in harmonics)) / abs(V1)
    print(f"THD: {thd:.4f}%")
    
    print("\n=== Harmonic Summary ===")
    print("Order  Magnitude(V)  % of Fundamental")
    print("--------------------------------------")
    print(f"1     {abs(V1):.6f}    100.0000%")
    for k, h in zip([3, 5, 7, 9, 11, 13, 15, 17, 19], harmonics):
        print(f"{k:2d}    {abs(h):.6f}    {100*abs(h)/abs(V1):.4f}%")