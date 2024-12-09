import numpy as np
import pandas as pd
from norm_4module import calculate_switching_angles_4modules

def generate_5d_lookup_table():
    # Define parameter ranges
    ci_options = np.array([0.95, 0.975, 1.0, 1.025, 1.05])
    m_options = np.arange(0.75, 1.01, 0.05)  # 0.75, 0.80, 0.85, 0.90, 0.95, 1.00
    
    # Create 5D arrays to store results
    shape = (len(ci_options), len(ci_options), len(ci_options), len(ci_options), len(m_options))
    theta1_table = np.zeros(shape)
    theta2_table = np.zeros(shape)
    theta3_table = np.zeros(shape)
    theta4_table = np.zeros(shape)
    thd_table = np.zeros(shape)
    v1_error_table = np.zeros(shape)
    
    total_combinations = np.prod(shape)
    print(f"Generating {total_combinations} combinations...")
    
    count = 0
    # Generate all combinations
    for i1, ci1 in enumerate(ci_options):
        for i2, ci2 in enumerate(ci_options):
            for i3, ci3 in enumerate(ci_options):
                for i4, ci4 in enumerate(ci_options):
                    for im, m in enumerate(m_options):
                        count += 1
                        if count % 100 == 0:
                            print(f"Processing combination {count}/{total_combinations}")
                        
                        # Create ci array and sort in descending order
                        ci_array = np.array([ci1, ci2, ci3, ci4])
                        ci_array = np.sort(ci_array)[::-1]
                        
                        # Calculate angles
                        angles = calculate_switching_angles_4modules(ci_array, m)
                        
                        # Calculate fundamental
                        V1 = float(sum(ci_array * np.cos(angles)))
                        V1_desired = m * np.pi
                        
                        # Calculate harmonics
                        harmonics = []
                        for k in [3, 5, 7, 9, 11, 13, 15, 17, 19]:
                            Vk = float(sum(ci_array * np.cos(k * angles)) / k)
                            harmonics.append(abs(Vk))
                        
                        # Calculate THD
                        thd = 100 * np.sqrt(sum(h*h for h in harmonics)) / abs(V1)
                        
                        # Store in 5D tables
                        theta1_table[i1,i2,i3,i4,im] = angles[0]
                        theta2_table[i1,i2,i3,i4,im] = angles[1]
                        theta3_table[i1,i2,i3,i4,im] = angles[2]
                        theta4_table[i1,i2,i3,i4,im] = angles[3]
                        thd_table[i1,i2,i3,i4,im] = thd
                        v1_error_table[i1,i2,i3,i4,im] = 100*(V1-V1_desired)/V1_desired
    
    # Save tables in binary format
    with open('switching_angles_table.bin', 'wb') as f:
        # Write dimensions first
        np.array([len(ci_options), len(m_options)], dtype=np.int32).tofile(f)
        # Write the parameter values
        ci_options.astype(np.float32).tofile(f)
        m_options.astype(np.float32).tofile(f)
        # Write the lookup tables
        theta1_table.astype(np.float32).tofile(f)
        theta2_table.astype(np.float32).tofile(f)
        theta3_table.astype(np.float32).tofile(f)
        theta4_table.astype(np.float32).tofile(f)
        thd_table.astype(np.float32).tofile(f)
        v1_error_table.astype(np.float32).tofile(f)
    
    print("\nTable dimensions:")
    print(f"ci values: {ci_options}")
    print(f"m values: {m_options}")
    print(f"Shape of tables: {shape}")
    
    # Example of how to use the table
    print("\nExample lookup:")
    i1, i2, i3, i4, im = 2, 2, 2, 2, 1  # middle values
    print(f"For ci=[1.0,1.0,1.0,1.0], m=0.80:")
    print(f"θ1 = {theta1_table[i1,i2,i3,i4,im]:.6f} rad")
    print(f"θ2 = {theta2_table[i1,i2,i3,i4,im]:.6f} rad")
    print(f"θ3 = {theta3_table[i1,i2,i3,i4,im]:.6f} rad")
    print(f"θ4 = {theta4_table[i1,i2,i3,i4,im]:.6f} rad")
    print(f"THD = {thd_table[i1,i2,i3,i4,im]:.4f}%")
    print(f"V1 error = {v1_error_table[i1,i2,i3,i4,im]:.4f}%")

if __name__ == "__main__":
    generate_5d_lookup_table() 