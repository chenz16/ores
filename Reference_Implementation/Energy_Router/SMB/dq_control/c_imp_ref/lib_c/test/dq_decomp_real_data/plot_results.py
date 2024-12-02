import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Read the CSV file
df = pd.read_csv('log_data_with_beta.csv')

# Print available columns for debugging
print("Available columns:", df.columns.tolist())

# Filter last 200ms of data
time_window = 0.2  # 200ms
last_time = df['time_s'].max()
start_time = last_time - time_window
df_filtered = df[df['time_s'] >= start_time].copy()

# Create figure with 4 subplots
fig, (ax1, ax2, ax3, ax4) = plt.subplots(4, 1, figsize=(12, 12))

# Plot 1: Alpha-Beta
ax1.plot(df_filtered['time_s'], df_filtered['current_value'], label='Alpha', color='blue')
ax1.plot(df_filtered['time_s'], df_filtered['beta'], label='Beta', color='red')
ax1.set_xlabel('Time (s)')
ax1.set_ylabel('Amplitude')
ax1.set_title('Alpha-Beta Components (Last 200ms)')
ax1.grid(True)
ax1.legend()

# Plot 2: D-Q components with filtered overlay
if all(col in df.columns for col in ['d', 'q', 'filtered_d', 'filtered_q']):
    # Plot unfiltered
    ax2.plot(df_filtered['time_s'], df_filtered['d'], label='D', color='green', alpha=0.5)
    ax2.plot(df_filtered['time_s'], df_filtered['q'], label='Q', color='purple', alpha=0.5)
    # Plot filtered
    ax2.plot(df_filtered['time_s'], df_filtered['filtered_d'], label='Filtered D', 
             color='green', linestyle='--', linewidth=2)
    ax2.plot(df_filtered['time_s'], df_filtered['filtered_q'], label='Filtered Q', 
             color='purple', linestyle='--', linewidth=2)
    ax2.set_xlabel('Time (s)')
    ax2.set_ylabel('Amplitude')
    ax2.set_title('D-Q Components - Filtered vs Unfiltered (Last 200ms)')
    ax2.grid(True)
    ax2.legend()

    # Plot 3: D-Q Vector Plot (only filtered data)
    ax3.plot(df_filtered['filtered_d'], df_filtered['filtered_q'], 'b-', 
             label='Filtered DQ trajectory')
    ax3.set_xlabel('D component')
    ax3.set_ylabel('Q component')
    ax3.set_title('Filtered D-Q Vector Plot (Last 200ms)')
    ax3.grid(True)
    ax3.set_aspect('equal')

    # Calculate magnitude for scaling (using filtered values)
    magnitude = np.sqrt(df_filtered['filtered_d']**2 + df_filtered['filtered_q']**2)
    mean_magnitude = np.mean(magnitude)

    # Add circle to show the expected trajectory
    circle = plt.Circle((0, 0), mean_magnitude, fill=False, linestyle='--', 
                       color='red', alpha=0.5, label='Mean magnitude')
    ax3.add_artist(circle)

    # Set axis limits for D-Q vector plot
    limit = mean_magnitude * 1.2
    ax3.set_xlim(-limit, limit)
    ax3.set_ylim(-limit, limit)
    ax3.legend()
else:
    # Display message if filtered data is not available
    for ax in [ax2, ax3]:
        ax.text(0.5, 0.5, 'Filtered D-Q data not available', 
                horizontalalignment='center',
                verticalalignment='center',
                transform=ax.transAxes)
        ax.set_xticks([])
        ax.set_yticks([])

# Add Plot 4: Angle Comparison
# ax4.plot(df_filtered['time_s'], df_filtered['current_angle'], label='Original Angle', color='blue')
ax4.plot(df_filtered['time_s'], df_filtered['computed_angle'], label='Computed Angle', 
         color='red', linestyle='--')
ax4.set_xlabel('Time (s)')
ax4.set_ylabel('Angle (x pi)')
ax4.set_title('Angle Comparison (Last 200ms)')
ax4.grid(True)
ax4.legend()

plt.tight_layout()
plt.savefig('dq_transform_results.png', dpi=300, bbox_inches='tight')
plt.show()