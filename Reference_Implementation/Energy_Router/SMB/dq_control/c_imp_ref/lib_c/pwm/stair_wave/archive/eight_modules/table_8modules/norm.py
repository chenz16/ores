import numpy as np
from iterative_search_angle import compare_original_and_paired_solutions, calculate_harmonics

def normalize_inputs(Vi, V1_desired=311):
    """
    Normalize voltage inputs and calculate modulation index.
    Args:
        Vi: List of 8 voltage sources [V]
        V1_desired: Desired peak AC output voltage (default 311V)
    Returns:
        ci: Normalized voltage coefficients
        m: Modulation index
    """
    V_avg = np.mean(Vi)
    Vdc_total = np.sum(Vi)
    
    # Normalize voltages by average voltage
    ci = Vi / V_avg
    
    # Calculate modulation index
    m = V1_desired / Vdc_total
    
    return ci, m

def denormalize_output(angles, m):
    """
    Denormalize the output angles.
    Args:
        angles: Switching angles in radians
        m: Modulation index
    Returns:
        denorm_angles: Denormalized switching angles
    """
    return angles * (8 * m * np.pi / 4)

if __name__ == "__main__":
    # Generate test case with random voltages (similar to original)
    mean_voltage = 50.0
    voltage_variation = 5.0
    V_modules = np.random.uniform(
        mean_voltage - voltage_variation,
        mean_voltage + voltage_variation,
        8
    )
    V_modules = np.round(V_modules, 1)
    V1_desired = 311

    # Original calculation for comparison
    angles_orig, angles_paired, V_paired, V_original = compare_original_and_paired_solutions(V_modules)
    V1_orig, h_orig = calculate_harmonics(angles_orig, V_original)
    V1_paired, h_paired = calculate_harmonics(angles_paired, V_original)

    # New normalized calculation
    ci, m = normalize_inputs(V_modules, V1_desired)
    
    print("\n=== Original Values ===")
    print(f"Original Voltages: {V_modules}")
    print(f"Desired V1: {V1_desired}V")
    
    print("\n=== Normalized Values ===")
    print(f"Normalized Coefficients (ci): {ci}")
    print(f"Modulation Index (m): {m:.3f}")
    print(f"Average Module Voltage: {np.mean(V_modules):.1f}V")
    print(f"Total DC Voltage: {np.sum(V_modules):.1f}V")

    # Print harmonics comparison
    print("\n=== Harmonics Comparison (V) ===")
    print("Order    Original    Paired")
    print("--------------------------------")
    for k, (ho, hp) in enumerate(zip(h_orig, h_paired)):
        harmonic = 2*k + 3
        print(f"{harmonic:3d}     {ho:7.2f}     {hp:7.2f}")
    
    # Calculate and print THD
    def calculate_thd(harmonics, V1):
        return 100 * np.sqrt(sum(h*h for h in harmonics)) / V1
    
    thd_orig = calculate_thd(h_orig, V1_orig)
    thd_paired = calculate_thd(h_paired, V1_paired)
    
    print("\n=== THD Comparison ===")
    print(f"Original THD: {thd_orig:.2f}%")
    print(f"Paired THD: {thd_paired:.2f}%")

    # If you want to verify the normalization:
    print("\n=== Verification ===")
    print(f"Sum of original Vi: {np.sum(V_modules):.1f}V")
    print(f"Sum of normalized ci * V_avg: {np.sum(ci * np.mean(V_modules)):.1f}V")

    print("\n\n==========================================")
    print("=== Direct ci and m Input Test Cases ===")
    print("==========================================")

    # Test case parameters
    m_test = 0.8  # Example modulation index
    c2_c8 = np.array([0.96, 0.98, 1.0, 1.01, 1.02, 1.03, 1.04])  # More conservative coefficients

    # Test Case 1: c1 = 0.95 (lower bound)
    print("\n=== Test Case 1: c1 = 0.95 ===")
    c1_test1 = 0.95
    ci_test1 = np.concatenate(([c1_test1], c2_c8))
    V_avg_test = 50.0  # Example average voltage
    V_test1 = ci_test1 * V_avg_test
    
    angles_orig1, angles_paired1, V_paired1, V_original1 = compare_original_and_paired_solutions(V_test1)
    V1_orig1, h_orig1 = calculate_harmonics(angles_orig1, V_original1)
    V1_paired1, h_paired1 = calculate_harmonics(angles_paired1, V_original1)

    print(f"\nInput Parameters:")
    print(f"ci values: {ci_test1}")
    print(f"m value: {m_test}")
    
    print("\n=== Harmonics Comparison (V) ===")
    print("Order    Original    Paired")
    print("--------------------------------")
    for k, (ho, hp) in enumerate(zip(h_orig1, h_paired1)):
        harmonic = 2*k + 3
        print(f"{harmonic:3d}     {ho:7.2f}     {hp:7.2f}")

    # Add detailed switching angle comparison
    print("\n=== Switching Angles Comparison (degrees) ===")
    print("\nTest Case 1 (c1 = 0.95):")
    print("Module    Voltage(Orig)    Voltage(Paired)    Orig_Angle    Paired_Angle")
    print("------------------------------------------------------------------------")
    for i, (vo, vp, ao, ap) in enumerate(zip(V_original1, V_paired1, 
                                            np.rad2deg(angles_orig1), 
                                            np.rad2deg(angles_paired1))):
        print(f"M{i+1:d}       {vo:4.1f}V          {vp:4.1f}V         {ao:6.2f}°      {ap:6.2f}°")

    # Test Case 2: c1 = 1.05 (upper bound)
    print("\n\n=== Test Case 2: c1 = 1.05 ===")
    c1_test2 = 1.05
    ci_test2 = np.concatenate(([c1_test2], c2_c8))
    V_test2 = ci_test2 * V_avg_test
    
    angles_orig2, angles_paired2, V_paired2, V_original2 = compare_original_and_paired_solutions(V_test2)
    V1_orig2, h_orig2 = calculate_harmonics(angles_orig2, V_original2)
    V1_paired2, h_paired2 = calculate_harmonics(angles_paired2, V_original2)

    print(f"\nInput Parameters:")
    print(f"ci values: {ci_test2}")
    print(f"m value: {m_test}")
    
    print("\n=== Harmonics Comparison (V) ===")
    print("Order    Original    Paired")
    print("--------------------------------")
    for k, (ho, hp) in enumerate(zip(h_orig2, h_paired2)):
        harmonic = 2*k + 3
        print(f"{harmonic:3d}     {ho:7.2f}     {hp:7.2f}")

    # Add detailed switching angle comparison
    print("\nTest Case 2 (c1 = 1.05):")
    print("Module    Voltage(Orig)    Voltage(Paired)    Orig_Angle    Paired_Angle")
    print("------------------------------------------------------------------------")
    for i, (vo, vp, ao, ap) in enumerate(zip(V_original2, V_paired2, 
                                            np.rad2deg(angles_orig2), 
                                            np.rad2deg(angles_paired2))):
        print(f"M{i+1:d}       {vo:4.1f}V          {vp:4.1f}V         {ao:6.2f}°      {ap:6.2f}°")

    # Compare THD for both test cases
    thd_orig1 = calculate_thd(h_orig1, V1_orig1)
    thd_paired1 = calculate_thd(h_paired1, V1_paired1)
    thd_orig2 = calculate_thd(h_orig2, V1_orig2)
    thd_paired2 = calculate_thd(h_paired2, V1_paired2)

    print("\n=== THD Comparison for Both Test Cases ===")
    print("Test Case 1 (c1 = 0.95):")
    print(f"THD (Original V):   {thd_orig1:5.2f}%")
    print(f"THD (Paired V):     {thd_paired1:5.2f}%")
    print("\nTest Case 2 (c1 = 1.05):")
    print(f"THD (Original V):   {thd_orig2:5.2f}%")
    print(f"THD (Paired V):     {thd_paired2:5.2f}%")
