#!/usr/bin/env python3
"""
Performance Analysis and Visualization for Parallel Prime Finder
Reads timing_manual.csv and generates comprehensive performance plots
"""

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os
import sys

def check_csv_exists():
    """Check if timing_manual.csv exists"""
    if not os.path.exists('timing_manual.csv'):
        print("Error: timing_manual.csv not found!")
        print("\nPlease run the C program first:")
        print("  gcc -o man_made man_made.c")
        print("  ./man_made")
        print("\nOr run with a larger range for better results:")
        print("  echo '1000000 10000000' | ./man_made")
        return False
    return True

def read_timing_data():
    """Read the CSV file with timing data"""
    try:
        df = pd.read_csv('timing_manual.csv')
        print(f"✓ Loaded {len(df)} data points from timing_manual.csv")
        print(f"  Processes tested: {df['Processes'].min()} to {df['Processes'].max()}")
        print(f"  Time range: {df['Time'].min():.6f}s to {df['Time'].max():.6f}s")
        return df
    except Exception as e:
        print(f"Error reading CSV: {e}")
        return None

def plot_execution_time(df):
    """Plot 1: Execution Time vs Number of Processes"""
    plt.figure(figsize=(10, 6))
    
    n_values = df['Processes'].values
    times = df['Time'].values
    
    # Plot the data
    plt.plot(n_values, times, 'b-o', linewidth=2, markersize=8, label='Execution Time')
    
    # Mark the optimal point
    min_idx = np.argmin(times)
    optimal_n = n_values[min_idx]
    optimal_time = times[min_idx]
    
    plt.plot(optimal_n, optimal_time, 'r*', markersize=20, 
             label=f'Optimal: n={optimal_n}, t={optimal_time:.4f}s')
    
    # Mark the number of cores
    max_n = n_values[-1]
    plt.axvline(x=max_n, color='green', linestyle='--', linewidth=2,
                label=f'Max cores tested ({max_n})')
    
    plt.xlabel('Number of Processes (n)', fontsize=12, fontweight='bold')
    plt.ylabel('Execution Time (seconds)', fontsize=12, fontweight='bold')
    plt.title('Parallel Prime Finder Performance\nExecution Time vs Number of Processes', 
              fontsize=14, fontweight='bold')
    plt.grid(True, alpha=0.3, linestyle='--')
    plt.legend(fontsize=10)
    plt.tight_layout()
    
    plt.savefig('plot1_execution_time.png', dpi=300, bbox_inches='tight')
    print("✓ Saved: plot1_execution_time.png")
    plt.close()

def plot_speedup(df):
    """Plot 2: Speedup vs Number of Processes"""
    plt.figure(figsize=(10, 6))
    
    n_values = df['Processes'].values
    times = df['Time'].values
    
    # Calculate speedup (T1 / Tn)
    baseline_time = times[0]  # Time with 1 process
    speedup = baseline_time / times
    
    # Ideal speedup (linear)
    ideal_speedup = n_values
    
    # Plot actual speedup
    plt.plot(n_values, speedup, 'b-o', linewidth=2, markersize=8, label='Actual Speedup')
    
    # Plot ideal speedup
    plt.plot(n_values, ideal_speedup, 'r--', linewidth=2, label='Ideal Speedup (Linear)')
    
    # Calculate and display efficiency at max cores
    max_idx = len(n_values) - 1
    efficiency = (speedup[max_idx] / n_values[max_idx]) * 100
    
    plt.annotate(f'Efficiency at n={n_values[max_idx]}:\n{efficiency:.1f}%',
                xy=(n_values[max_idx], speedup[max_idx]),
                xytext=(n_values[max_idx]*0.7, speedup[max_idx]*1.2),
                arrowprops=dict(arrowstyle='->', color='green', lw=2),
                fontsize=10, color='green', fontweight='bold',
                bbox=dict(boxstyle='round,pad=0.5', facecolor='yellow', alpha=0.3))
    
    plt.xlabel('Number of Processes (n)', fontsize=12, fontweight='bold')
    plt.ylabel('Speedup (T₁/Tₙ)', fontsize=12, fontweight='bold')
    plt.title('Speedup Analysis\nActual vs Ideal Speedup', 
              fontsize=14, fontweight='bold')
    plt.grid(True, alpha=0.3, linestyle='--')
    plt.legend(fontsize=10)
    plt.tight_layout()
    
    plt.savefig('plot2_speedup.png', dpi=300, bbox_inches='tight')
    print("✓ Saved: plot2_speedup.png")
    plt.close()
    
    return speedup

def plot_efficiency(df, speedup):
    """Plot 3: Parallel Efficiency vs Number of Processes"""
    plt.figure(figsize=(10, 6))
    
    n_values = df['Processes'].values
    
    # Calculate efficiency (Speedup / n) * 100%
    efficiency = (speedup / n_values) * 100
    
    plt.plot(n_values, efficiency, 'g-o', linewidth=2, markersize=8, label='Parallel Efficiency')
    plt.axhline(y=100, color='r', linestyle='--', linewidth=2, label='Ideal Efficiency (100%)')
    plt.axhline(y=80, color='orange', linestyle=':', linewidth=1.5, label='Good Efficiency (80%)')
    
    plt.xlabel('Number of Processes (n)', fontsize=12, fontweight='bold')
    plt.ylabel('Parallel Efficiency (%)', fontsize=12, fontweight='bold')
    plt.title('Parallel Efficiency Analysis\n(Speedup/n) × 100%', 
              fontsize=14, fontweight='bold')
    plt.grid(True, alpha=0.3, linestyle='--')
    plt.legend(fontsize=10)
    plt.ylim([0, min(110, max(efficiency) * 1.1)])
    plt.tight_layout()
    
    plt.savefig('plot3_efficiency.png', dpi=300, bbox_inches='tight')
    print("✓ Saved: plot3_efficiency.png")
    plt.close()

def plot_combined_analysis(df, speedup):
    """Plot 4: Combined Analysis - Time, Speedup, and Efficiency"""
    fig, axes = plt.subplots(2, 2, figsize=(14, 10))
    
    n_values = df['Processes'].values
    times = df['Time'].values
    efficiency = (speedup / n_values) * 100
    
    # Plot 1: Execution Time
    ax1 = axes[0, 0]
    ax1.plot(n_values, times, 'b-o', linewidth=2, markersize=6)
    min_idx = np.argmin(times)
    ax1.plot(n_values[min_idx], times[min_idx], 'r*', markersize=15)
    ax1.set_xlabel('Number of Processes', fontweight='bold')
    ax1.set_ylabel('Time (seconds)', fontweight='bold')
    ax1.set_title('Execution Time', fontweight='bold')
    ax1.grid(True, alpha=0.3)
    
    # Plot 2: Speedup
    ax2 = axes[0, 1]
    ax2.plot(n_values, speedup, 'g-o', linewidth=2, markersize=6, label='Actual')
    ax2.plot(n_values, n_values, 'r--', linewidth=2, label='Ideal')
    ax2.set_xlabel('Number of Processes', fontweight='bold')
    ax2.set_ylabel('Speedup', fontweight='bold')
    ax2.set_title('Speedup Analysis', fontweight='bold')
    ax2.legend()
    ax2.grid(True, alpha=0.3)
    
    # Plot 3: Efficiency
    ax3 = axes[1, 0]
    ax3.plot(n_values, efficiency, 'm-o', linewidth=2, markersize=6)
    ax3.axhline(y=100, color='r', linestyle='--', linewidth=1.5)
    ax3.set_xlabel('Number of Processes', fontweight='bold')
    ax3.set_ylabel('Efficiency (%)', fontweight='bold')
    ax3.set_title('Parallel Efficiency', fontweight='bold')
    ax3.grid(True, alpha=0.3)
    
    # Plot 4: Summary Statistics Table
    ax4 = axes[1, 1]
    ax4.axis('off')
    
    # Create summary table
    summary_data = [
        ['Metric', 'Value'],
        ['Baseline Time (n=1)', f'{times[0]:.4f} s'],
        ['Best Time', f'{times[min_idx]:.4f} s'],
        ['Optimal Processes', f'{n_values[min_idx]}'],
        ['Max Speedup', f'{speedup.max():.2f}x'],
        ['Best Efficiency', f'{efficiency.max():.1f}%'],
        ['Efficiency at Max n', f'{efficiency[-1]:.1f}%'],
        ['Processes Tested', f'{len(n_values)}']
    ]
    
    table = ax4.table(cellText=summary_data, cellLoc='left', loc='center',
                     colWidths=[0.6, 0.4])
    table.auto_set_font_size(False)
    table.set_fontsize(10)
    table.scale(1, 2)
    
    # Style header row
    for i in range(2):
        table[(0, i)].set_facecolor('#4CAF50')
        table[(0, i)].set_text_props(weight='bold', color='white')
    
    # Alternate row colors
    for i in range(1, len(summary_data)):
        for j in range(2):
            if i % 2 == 0:
                table[(i, j)].set_facecolor('#f0f0f0')
    
    ax4.set_title('Performance Summary', fontweight='bold', fontsize=12, pad=20)
    
    plt.suptitle('Comprehensive Performance Analysis', 
                 fontsize=16, fontweight='bold', y=0.98)
    plt.tight_layout(rect=[0, 0, 1, 0.96])
    
    plt.savefig('plot4_combined_analysis.png', dpi=300, bbox_inches='tight')
    print("✓ Saved: plot4_combined_analysis.png")
    plt.close()

def plot_overhead_analysis(df):
    """Plot 5: Overhead Analysis"""
    plt.figure(figsize=(10, 6))
    
    n_values = df['Processes'].values
    times = df['Time'].values
    
    # Calculate theoretical ideal time (assuming perfect parallelization)
    baseline_time = times[0]
    ideal_times = baseline_time / n_values
    
    # Calculate overhead
    overhead = times - ideal_times
    overhead_percent = (overhead / ideal_times) * 100
    
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 5))
    
    # Left plot: Actual vs Ideal time
    ax1.plot(n_values, times, 'b-o', linewidth=2, markersize=8, label='Actual Time')
    ax1.plot(n_values, ideal_times, 'r--', linewidth=2, label='Ideal Time (No Overhead)')
    ax1.fill_between(n_values, ideal_times, times, alpha=0.3, color='red', label='Overhead')
    ax1.set_xlabel('Number of Processes', fontsize=12, fontweight='bold')
    ax1.set_ylabel('Time (seconds)', fontsize=12, fontweight='bold')
    ax1.set_title('Actual vs Ideal Execution Time', fontweight='bold')
    ax1.legend()
    ax1.grid(True, alpha=0.3)
    
    # Right plot: Overhead percentage
    ax2.plot(n_values, overhead_percent, 'r-o', linewidth=2, markersize=8)
    ax2.set_xlabel('Number of Processes', fontsize=12, fontweight='bold')
    ax2.set_ylabel('Overhead (%)', fontsize=12, fontweight='bold')
    ax2.set_title('Parallelization Overhead', fontweight='bold')
    ax2.grid(True, alpha=0.3)
    
    plt.suptitle('Overhead Analysis: Impact of Process Management', 
                 fontsize=14, fontweight='bold')
    plt.tight_layout()
    
    plt.savefig('plot5_overhead_analysis.png', dpi=300, bbox_inches='tight')
    print("✓ Saved: plot5_overhead_analysis.png")
    plt.close()

def print_analysis_report(df, speedup):
    """Print detailed analysis report"""
    n_values = df['Processes'].values
    times = df['Time'].values
    efficiency = (speedup / n_values) * 100
    
    print("\n" + "="*60)
    print("PERFORMANCE ANALYSIS REPORT")
    print("="*60)
    
    print(f"\n📊 EXECUTION TIME ANALYSIS:")
    print(f"  Sequential (n=1):     {times[0]:.6f} seconds")
    print(f"  Best Time:            {times.min():.6f} seconds")
    min_idx = np.argmin(times)
    print(f"  Optimal Processes:    {n_values[min_idx]}")
    print(f"  Worst Time:           {times.max():.6f} seconds")
    
    print(f"\n🚀 SPEEDUP ANALYSIS:")
    print(f"  Maximum Speedup:      {speedup.max():.2f}x")
    print(f"  Speedup at n={n_values[-1]}:       {speedup[-1]:.2f}x")
    print(f"  Average Speedup:      {speedup.mean():.2f}x")
    
    print(f"\n⚡ EFFICIENCY ANALYSIS:")
    print(f"  Best Efficiency:      {efficiency.max():.1f}%")
    print(f"  Efficiency at n={n_values[-1]}:    {efficiency[-1]:.1f}%")
    print(f"  Average Efficiency:   {efficiency.mean():.1f}%")
    
    # Amdahl's Law estimation
    if len(speedup) > 1:
        # Estimate sequential portion
        s_max = speedup.max()
        p = (s_max - 1) / s_max  # Parallel portion
        serial = 1 - p
        print(f"\n📐 AMDAHL'S LAW ESTIMATION:")
        print(f"  Estimated Parallel:   {p*100:.1f}%")
        print(f"  Estimated Sequential: {serial*100:.1f}%")
        print(f"  Theoretical Max:      {1/serial:.2f}x speedup")
    
    print("\n" + "="*60)

def main():
    """Main function to generate all plots"""
    print("="*60)
    print("PARALLEL PRIME FINDER - GRAPH GENERATOR")
    print("="*60)
    print()
    
    # Check if CSV exists
    if not check_csv_exists():
        return
    
    # Read data
    df = read_timing_data()
    if df is None:
        return
    
    print("\n📈 Generating plots...")
    print("-"*60)
    
    # Generate all plots
    plot_execution_time(df)
    speedup = plot_speedup(df)
    plot_efficiency(df, speedup)
    plot_combined_analysis(df, speedup)
    plot_overhead_analysis(df)
    
    # Print analysis report
    print_analysis_report(df, speedup)
    
    print("\n✅ ALL PLOTS GENERATED SUCCESSFULLY!")
    print("\nGenerated files:")
    print("  1. plot1_execution_time.png    - Time vs Processes")
    print("  2. plot2_speedup.png            - Speedup analysis")
    print("  3. plot3_efficiency.png         - Efficiency analysis")
    print("  4. plot4_combined_analysis.png  - Comprehensive view")
    print("  5. plot5_overhead_analysis.png  - Overhead analysis")
    print("\n" + "="*60)

if __name__ == '__main__':
    main()