import pandas as pd
import matplotlib.pyplot as plt

# Read the CSV file
data = pd.read_csv('simulation_results.csv')

# Create subplots
fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(10, 12))

# Time axis
t = data['time_stamp'].values  # Changed from 't' to 'time_stamp'

# Plot currents
ax1.plot(t, data['i_raw_d'], label='i_d raw')
ax1.plot(t, data['i_notch_d'], label='i_d notch')
ax1.plot(t, data['i_filtered_d'], label='i_d filtered')
ax1.set_xlabel('Time (s)')
ax1.set_ylabel('Current (A)')
ax1.set_title('D-axis Current')
ax1.grid(True)
ax1.legend()

# Plot q-axis currents
ax2.plot(t, data['i_raw_q'], label='i_q raw')
ax2.plot(t, data['i_notch_q'], label='i_q notch')
ax2.plot(t, data['i_filtered_q'], label='i_q filtered')
ax2.set_xlabel('Time (s)')
ax2.set_ylabel('Current (A)')
ax2.set_title('Q-axis Current')
ax2.grid(True)
ax2.legend()

# Plot grid voltage
ax3.plot(t, data['v_grid_d'], label='v_grid_d')
ax3.plot(t, data['v_grid_q'], label='v_grid_q')
ax3.set_xlabel('Time (s)')
ax3.set_ylabel('Voltage (V)')
ax3.set_title('Grid Voltage')
ax3.grid(True)
ax3.legend()

plt.tight_layout()
plt.savefig('simulation_results.png')
plt.show()
plt.close()