import numpy as np
from scipy.interpolate import interpn
from norm_4module import calculate_switching_angles_4modules

def load_lookup_table(filename='switching_angles_5d_table.npz'):
    data = np.load(filename)
    return (data['ci_values'], data['m_values'], 
            data['theta1'], data['theta2'], 
            data['theta3'], data['theta4'])

def interpolate_angles_linear(ci_values, m_values, theta_tables, query_point):
    """Linear interpolation"""
    points = (ci_values, ci_values, ci_values, ci_values, m_values)
    angles = []
    for theta_table in theta_tables:
        angle = interpn(points, theta_table, query_point.reshape(1,-1), 
                       method='linear')[0]
        angles.append(angle)
    return np.array(angles)

def interpolate_angles_manual_linear(ci_values, m_values, theta_tables, query_point):
    """Manual linear interpolation using Taylor expansion approach"""
    # Find nearest lower indices for each dimension
    ci_indices = []
    for ci in query_point[:-1]:  # Handle ci values
        idx = np.searchsorted(ci_values, ci) - 1
        idx = max(0, min(idx, len(ci_values) - 2))  # Ensure we have room for interpolation
        ci_indices.append(idx)
    
    m_idx = np.searchsorted(m_values, query_point[-1]) - 1
    m_idx = max(0, min(m_idx, len(m_values) - 2))
    
    # Get deltas (distances from lower point)
    ci_deltas = [query_point[i] - ci_values[ci_indices[i]] for i in range(4)]
    m_delta = query_point[-1] - m_values[m_idx]
    
    angles = []
    for theta_table in theta_tables:
        # Base point value
        base_value = theta_table[ci_indices[0], ci_indices[1], 
                               ci_indices[2], ci_indices[3], m_idx]
        
        # Partial derivatives for each dimension
        partials = []
        # For each ci dimension
        for dim in range(4):
            idx_high = list(ci_indices)
            idx_high[dim] += 1
            partial_ci = (theta_table[tuple(idx_high + [m_idx])] - 
                         theta_table[tuple(ci_indices + [m_idx])]) / (ci_values[ci_indices[dim] + 1] - ci_values[ci_indices[dim]])
            partials.append(partial_ci)
        
        # For m dimension
        partial_m = (theta_table[tuple(ci_indices + [m_idx + 1])] - 
                    theta_table[tuple(ci_indices + [m_idx])]) / (m_values[m_idx + 1] - m_values[m_idx])
        
        # Apply Taylor expansion
        interpolated_value = base_value
        for i in range(4):
            interpolated_value += partials[i] * ci_deltas[i]
        interpolated_value += partial_m * m_delta
        
        angles.append(interpolated_value)
    
    return np.array(angles)


def interpolate_angles_cubic(ci_values, m_values, theta_tables, query_point):
    """Cubic interpolation"""
    points = (ci_values, ci_values, ci_values, ci_values, m_values)
    angles = []
    for theta_table in theta_tables:
        angle = interpn(points, theta_table, query_point.reshape(1,-1), 
                       method='cubic', bounds_error=False, fill_value=None)[0]
        angles.append(angle)
    return np.array(angles)

def calculate_harmonics_and_thd(ci_array, angles):
    V1 = float(sum(ci_array * np.cos(angles)))
    harmonics = []
    for k in [3, 5, 7, 9, 11, 13, 15, 17, 19]:
        Vk = float(sum(ci_array * np.cos(k * angles)) / k)
        harmonics.append(abs(Vk))
    thd = 100 * np.sqrt(sum(h*h for h in harmonics)) / abs(V1)
    return V1, harmonics, thd

def print_comparison_table(ci_array, m, angles_linear, angles_cubic, angles_manual, angles_direct, V1_desired):
    V1_linear, harmonics_linear, thd_linear = calculate_harmonics_and_thd(ci_array, angles_linear)
    V1_cubic, harmonics_cubic, thd_cubic = calculate_harmonics_and_thd(ci_array, angles_cubic)
    V1_manual, harmonics_manual, thd_manual = calculate_harmonics_and_thd(ci_array, angles_manual)
    V1_direct, harmonics_direct, thd_direct = calculate_harmonics_and_thd(ci_array, angles_direct)
    
    print("\n=== Comparison Results ===")
    print(f"ci values: {ci_array}")
    print(f"m value: {m:.3f}")
    print(f"V1 desired: {V1_desired:.6f}")
    
    print("\nSwitching Angles:")
    print("Angle    Linear (rad)   Manual (rad)   Cubic (rad)    Direct (rad)   Lin-Dir(deg)  Man-Dir(deg)  Cub-Dir(deg)")
    print("-" * 105)
    for i, (al, am, ac, ad) in enumerate(zip(angles_linear, angles_manual, angles_cubic, angles_direct)):
        print(f"θ{i+1}      {al:12.6f}  {am:12.6f}  {ac:12.6f}  {ad:12.6f}  {np.degrees(al-ad):10.4f}  {np.degrees(am-ad):10.4f}  {np.degrees(ac-ad):10.4f}")
    
    print("\nFundamental and THD:")
    print("Parameter           Linear         Manual         Cubic          Direct         Lin-Dir        Man-Dir        Cub-Dir")
    print("-" * 105)
    print(f"V1              {V1_linear:12.6f}  {V1_manual:12.6f}  {V1_cubic:12.6f}  {V1_direct:12.6f}  {V1_linear-V1_direct:12.6f}  {V1_manual-V1_direct:12.6f}  {V1_cubic-V1_direct:12.6f}")
    print(f"V1 Error (%)    {100*(V1_linear-V1_desired)/V1_desired:12.6f}  {100*(V1_manual-V1_desired)/V1_desired:12.6f}  {100*(V1_cubic-V1_desired)/V1_desired:12.6f}  {100*(V1_direct-V1_desired)/V1_desired:12.6f}")
    print(f"THD (%)         {thd_linear:12.6f}  {thd_manual:12.6f}  {thd_cubic:12.6f}  {thd_direct:12.6f}  {thd_linear-thd_direct:12.6f}  {thd_manual-thd_direct:12.6f}  {thd_cubic-thd_direct:12.6f}")
    
    print("\nHarmonics:")
    print("Order  Linear (V)   % Fund   Manual (V)   % Fund   Cubic (V)    % Fund   Direct (V)   % Fund   Lin-Dir(%)  Man-Dir(%)  Cub-Dir(%)")
    print("-" * 120)
    for k, (hl, hm, hc, hd) in zip([1,3,5,7,9,11,13,15,17,19], 
                                  zip([V1_linear] + harmonics_linear,
                                      [V1_manual] + harmonics_manual, 
                                      [V1_cubic] + harmonics_cubic, 
                                      [V1_direct] + harmonics_direct)):
        if k == 1:
            print(f"{k:2d}   {abs(hl):10.6f}  100.000  {abs(hm):10.6f}  100.000  {abs(hc):10.6f}  100.000  {abs(hd):10.6f}  100.000    0.0000     0.0000     0.0000")
        else:
            print(f"{k:2d}   {abs(hl):10.6f}  {100*abs(hl)/abs(V1_linear):7.4f}  {abs(hm):10.6f}  {100*abs(hm)/abs(V1_manual):7.4f}  "
                  f"{abs(hc):10.6f}  {100*abs(hc)/abs(V1_cubic):7.4f}  {abs(hd):10.6f}  {100*abs(hd)/abs(V1_direct):7.4f}  "
                  f"{100*(abs(hl)/abs(V1_linear) - abs(hd)/abs(V1_direct)):9.4f}  {100*(abs(hm)/abs(V1_manual) - abs(hd)/abs(V1_direct)):9.4f}  "
                  f"{100*(abs(hc)/abs(V1_cubic) - abs(hd)/abs(V1_direct)):9.4f}")

def test_lookup_table():
    ci_values, m_values, theta1, theta2, theta3, theta4 = load_lookup_table()
    theta_tables = [theta1, theta2, theta3, theta4]
    
    print("=== Testing Linear vs Cubic Interpolation ===")
    print("Testing interpolation points between 0.975 and 1.025")
    
    interpolation_points = [
        (np.array([0.99, 0.995, 1.01, 1.02]), 0.827),    # Between table points
        (np.array([0.98, 1.0, 1.015, 1.02]), 0.873),     # Mix with exact 1.0
        (np.array([0.985, 0.995, 1.005, 1.015]), 0.92),  # Symmetric around 1.0
        (np.array([0.98, 0.99, 1.01, 1.02]), 0.777),     # Even spacing
        (np.array([0.985, 1.0, 1.01, 1.015]), 0.968)     # Close to nominal
    ]
    
    for i, (ci_interp, m_interp) in enumerate(interpolation_points, 1):
        print(f"\nTest Case {i}: Interpolation point")
        V1_desired_interp = m_interp * np.pi
        query_interp = np.array([*ci_interp, m_interp])
        
        # Get angles from all methods
        angles_linear = interpolate_angles_linear(ci_values, m_values, theta_tables, query_interp)
        angles_manual = interpolate_angles_manual_linear(ci_values, m_values, theta_tables, query_interp)
        angles_cubic = interpolate_angles_cubic(ci_values, m_values, theta_tables, query_interp)
        angles_direct = calculate_switching_angles_4modules(ci_interp, m_interp)
        
        print_comparison_table(ci_interp, m_interp, angles_linear, angles_cubic, angles_manual, angles_direct, V1_desired_interp)
    
    # Time comparison
    import time
    
    t0 = time.time()
    for _ in range(100):
        angles_linear = interpolate_angles_linear(ci_values, m_values, theta_tables, query_interp)
    linear_time = (time.time() - t0) / 100
    
    t0 = time.time()
    for _ in range(100):
        angles_cubic = interpolate_angles_cubic(ci_values, m_values, theta_tables, query_interp)
    cubic_time = (time.time() - t0) / 100
    
    t0 = time.time()
    for _ in range(100):
        angles_direct = calculate_switching_angles_4modules(ci_interp, m_interp)
    direct_time = (time.time() - t0) / 100
    
    t0 = time.time()
    for _ in range(100):
        angles_manual = interpolate_angles_manual_linear(ci_values, m_values, theta_tables, query_interp)
    manual_time = (time.time() - t0) / 100
    
    print("\nExecution Time Comparison (average over 100 runs):")
    print(f"Built-in linear interpolation: {linear_time*1000:.3f} ms")
    print(f"Manual linear interpolation: {manual_time*1000:.3f} ms")
    print(f"Cubic interpolation: {cubic_time*1000:.3f} ms")
    print(f"Direct calculation: {direct_time*1000:.3f} ms")
    print(f"Built-in linear speedup factor: {direct_time/linear_time:.1f}x")
    print(f"Manual linear speedup factor: {direct_time/manual_time:.1f}x")
    print(f"Cubic speedup factor: {direct_time/cubic_time:.1f}x")

if __name__ == "__main__":
    test_lookup_table()