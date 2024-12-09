import numpy as np
from casadi_opti import optimize_switching_angles

def calculate_harmonics(angles, voltages):
    """
    Calculate fundamental and harmonic components.
    Args:
        angles: Switching angles in radians
        voltages: DC voltage levels
    Returns:
        V1: Fundamental component
        harmonics: List of harmonic components
    """
    # Calculate fundamental (k=1)
    V1 = abs((4 / np.pi) * sum(v * np.cos(a) for v, a in zip(voltages, angles)))
    
    # Calculate higher harmonics
    harmonics = []
    for k in [3, 5, 7, 9, 11, 13, 15]:
        Vk = abs((4 / (k * np.pi)) * sum(v * np.cos(k * a) 
             for v, a in zip(voltages, angles)))
        harmonics.append(Vk)
    return V1, harmonics

def compare_original_and_paired_solutions(Vi, V1_desired=311):
    """
    Compare original solution with paired voltage approximation.
    Args:
        Vi: List of 8 voltage sources [V]
        V1_desired: Desired peak AC output voltage (default 311V)
    Returns:
        angles_original: Switching angles from original solution
        angles_paired: Switching angles from paired solution
        V_paired: Averaged voltage levels
        V_original: Original voltage levels
    """
    # Sort voltages in ascending order
    Vi = np.sort(Vi)
    V_original = Vi.copy()
    total_voltage = np.sum(Vi)
    
    print("\nDebug - Before optimization:")
    print("Original Vi (sorted):", Vi)
    print("Total DC voltage (original):", total_voltage)
    
    # Get original solution first using original voltages
    angles_original = optimize_switching_angles(V_original, V1_desired)
    
    # Create paired voltage array (8 levels with paired values)
    V_paired = np.array([
        (Vi[0] + Vi[1]) / 2,  # V1=V2 (first pair)
        (Vi[0] + Vi[1]) / 2,
        (Vi[2] + Vi[3]) / 2,  # V3=V4 (second pair)
        (Vi[2] + Vi[3]) / 2,
        (Vi[4] + Vi[5]) / 2,  # V5=V6 (third pair)
        (Vi[4] + Vi[5]) / 2,
        (Vi[6] + Vi[7]) / 2,  # V7=V8 (fourth pair)
        (Vi[6] + Vi[7]) / 2
    ])
    
    print("V_paired:", V_paired)
    print("Total DC voltage (paired):", np.sum(V_paired))
    
    # Get paired solution
    angles_paired = optimize_switching_angles(V_paired, V1_desired)
    
    return angles_original, angles_paired, V_paired, V_original

if __name__ == "__main__":
    # Test case: Generate random voltages
    mean_voltage = 50.0
    voltage_variation = 5.0
    V_modules = np.random.uniform(
        mean_voltage - voltage_variation,
        mean_voltage + voltage_variation,
        8
    )
    V_modules = np.round(V_modules, 1)
    
    # Get both solutions and analyze results
    angles_orig, angles_paired, V_paired, V_original = compare_original_and_paired_solutions(V_modules)
    
    # Calculate harmonics for both cases
    # For original solution: use original angles with original voltages
    V1_orig, h_orig = calculate_harmonics(angles_orig, V_original)
    
    # For paired solution: use paired angles with original voltages to see real effect
    V1_paired, h_paired = calculate_harmonics(angles_paired, V_original)  # Use V_original here!
    
    print("\n=== Original DC Module Voltages ===")
    for i, v in enumerate(V_original):
        print(f"V{i+1}: {v:.1f}V")
    
    print("\n=== Paired DC Module Voltages ===")
    for i in range(0, 8, 2):
        avg_v = V_paired[i]
        print(f"V{i+1} = V{i+2} = {avg_v:.1f}V")
    
    print("\n=== Switching Angles (degrees) ===")
    print("Module    Voltage(Orig)    Voltage(Paired)    Angle")
    print("---------------------------------------------------")
    for i, (vo, vp, ac) in enumerate(zip(V_original, V_paired, np.rad2deg(angles_paired))):
        print(f"M{i+1:d}       {vo:4.1f}V          {vp:4.1f}V         {ac:6.2f}")
    
    V1_desired = 311
    print(f"\n=== Fundamental Component Comparison ===")
    print(f"Desired:            {V1_desired:7.2f}V")
    print(f"Original solution:  {V1_orig:7.2f}V  (Error: {100*(V1_orig-V1_desired)/V1_desired:6.2f}%)")
    print(f"With paired V:      {V1_paired:7.2f}V  (Error: {100*(V1_paired-V1_desired)/V1_desired:6.2f}%)")
    
    print("\n=== Harmonics Comparison (V) ===")
    print("Order    Original    Paired")
    print("--------------------------------")
    for k, (ho, hp) in enumerate(zip(h_orig, h_paired)):
        harmonic = 2*k + 3
        print(f"{harmonic:3d}     {ho:7.2f}     {hp:7.2f}")
    
    def calculate_thd(harmonics, V1):
        return 100 * np.sqrt(sum(h*h for h in harmonics)) / V1
    
    thd_orig = calculate_thd(h_orig, V1_orig)
    thd_paired = calculate_thd(h_paired, V1_paired)
    
    print("\n=== THD Comparison ===")
    print(f"THD (Original V):   {thd_orig:5.2f}%")
    print(f"THD (Paired V):     {thd_paired:5.2f}%")
