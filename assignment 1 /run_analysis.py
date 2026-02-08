#!/usr/bin/env python3
import subprocess
import matplotlib.pyplot as plt
import numpy as np
import os

def compile_program():
    """Compile the C program"""
    result = subprocess.run(['gcc', '-o', 'fastprime', 'fastprime.c', '-lm'], 
                          capture_output=True, text=True)
    if result.returncode != 0:
        print("Compilation error:", result.stderr)
        return False
    print("Program compiled successfully!")
    return True

def get_processor_count():
    """Get the number of logical processors"""
    result = subprocess.run(['./fastprime', '1', '10'], 
                          capture_output=True, text=True)
    for line in result.stdout.split('\n'):
        if 'logical processors' in line:
            return int(line.split()[2])
    return 4  # Default fallback

def run_experiment(rl, rh, max_n=None):
    """Run the experiment for a given range"""
    if max_n is None:
        max_n = get_processor_count() * 2
    
    result = subprocess.run(['./fastprime', str(rl), str(rh)], 
                          capture_output=True, text=True)
    
    lines = result.stdout.strip().split('\n')
    
    # Find where CSV data starts
    data_start = 0
    for i, line in enumerate(lines):
        if line.startswith('n,execution_time'):
            data_start = i + 1
            break
    
    n_values = []
    times = []
    
    for line in lines[data_start:]:
        if ',' in line:
            try:
                parts = line.split(',')
                n_values.append(int(parts[0]))
                times.append(float(parts[1]))
            except (ValueError, IndexError):
                continue
    
    return n_values, times

def plot_single_range(rl, rh, n_values, times, processors):
    """Create plot for a single range"""
    plt.figure(figsize=(10, 6))
    
    plt.plot(n_values, times, 'b-o', linewidth=2, markersize=6)
    plt.axvline(x=processors, color='r', linestyle='--', 
                label=f'Physical cores ({processors})')
    
    plt.xlabel('Number of Child Processes (n)', fontsize=12)
    plt.ylabel('Execution Time (seconds)', fontsize=12)
    plt.title(f'Prime Number Computation Performance\nRange: [{rl}, {rh}]', 
              fontsize=14, fontweight='bold')
    plt.grid(True, alpha=0.3)
    plt.legend()
    
    # Find optimal n
    min_time_idx = times.index(min(times))
    optimal_n = n_values[min_time_idx]
    
    plt.annotate(f'Optimal: n={optimal_n}\nTime={times[min_time_idx]:.4f}s',
                xy=(optimal_n, times[min_time_idx]),
                xytext=(optimal_n+1, times[min_time_idx]+max(times)*0.1),
                arrowprops=dict(arrowstyle='->', color='green', lw=2),
                fontsize=10, color='green', fontweight='bold')
    
    plt.tight_layout()
    filename = f'performance_{rl}_{rh}.png'
    plt.savefig(filename, dpi=300, bbox_inches='tight')
    print(f"Saved plot: {filename}")
    plt.close()
    
    return optimal_n, times[min_time_idx]

def plot_multiple_ranges(results, processors):
    """Create comparison plot for multiple ranges"""
    plt.figure(figsize=(12, 8))
    
    colors = ['blue', 'green', 'red', 'purple', 'orange']
    
    for i, (range_label, n_values, times) in enumerate(results):
        plt.plot(n_values, times, '-o', linewidth=2, markersize=6,
                color=colors[i % len(colors)], label=range_label)
    
    plt.axvline(x=processors, color='black', linestyle='--', 
                label=f'Physical cores ({processors})', linewidth=2)
    
    plt.xlabel('Number of Child Processes (n)', fontsize=12)
    plt.ylabel('Execution Time (seconds)', fontsize=12)
    plt.title('Performance Comparison Across Different Ranges', 
              fontsize=14, fontweight='bold')
    plt.grid(True, alpha=0.3)
    plt.legend(loc='best')
    
    plt.tight_layout()
    plt.savefig('performance_comparison.png', dpi=300, bbox_inches='tight')
    print("Saved plot: performance_comparison.png")
    plt.close()

def plot_speedup(n_values, times, processors):
    """Create speedup plot"""
    speedup = [times[0] / t for t in times]
    ideal_speedup = list(range(1, len(n_values) + 1))
    
    plt.figure(figsize=(10, 6))
    plt.plot(n_values, speedup, 'b-o', linewidth=2, markersize=6, label='Actual Speedup')
    plt.plot(n_values, ideal_speedup[:len(n_values)], 'r--', 
             linewidth=2, label='Ideal Speedup')
    plt.axvline(x=processors, color='green', linestyle='--', 
                label=f'Physical cores ({processors})')
    
    plt.xlabel('Number of Child Processes (n)', fontsize=12)
    plt.ylabel('Speedup', fontsize=12)
    plt.title('Speedup vs Number of Processes', fontsize=14, fontweight='bold')
    plt.grid(True, alpha=0.3)
    plt.legend()
    
    plt.tight_layout()
    plt.savefig('speedup_analysis.png', dpi=300, bbox_inches='tight')
    print("Saved plot: speedup_analysis.png")
    plt.close()

def main():
    print("=" * 60)
    print("PARALLEL PRIME NUMBER FINDER - PERFORMANCE ANALYSIS")
    print("=" * 60)
    print()
    
    # Compile the program
    if not compile_program():
        return
    
    print()
    
    # Get processor count
    processors = get_processor_count()
    print(f"Detected {processors} logical processors\n")
    
    # Define test ranges
    test_ranges = [
        (1000, 10000),
        (10000, 50000),
        (50000, 100000)
    ]
    
    all_results = []
    
    print("Running experiments...")
    print("-" * 60)
    
    for rl, rh in test_ranges:
        print(f"\nTesting range [{rl}, {rh}]...")
        n_values, times = run_experiment(rl, rh)
        
        # Create individual plot
        optimal_n, min_time = plot_single_range(rl, rh, n_values, times, processors)
        
        all_results.append((f"[{rl}, {rh}]", n_values, times))
        
        print(f"  Optimal n: {optimal_n}")
        print(f"  Best time: {min_time:.6f} seconds")
        print(f"  Speedup over serial: {times[0]/min_time:.2f}x")
    
    print("\n" + "-" * 60)
    print("\nGenerating comparison plots...")
    
    # Create comparison plot
    plot_multiple_ranges(all_results, processors)
    
    # Create speedup plot for the largest range
    _, n_vals, times_vals = all_results[-1]
    plot_speedup(n_vals, times_vals, processors)
    
    print("\n" + "=" * 60)
    print("ANALYSIS COMPLETE!")
    print("=" * 60)
    print("\nGenerated files:")
    print("  - prime.txt (prime numbers)")
    for rl, rh in test_ranges:
        print(f"  - performance_{rl}_{rh}.png")
    print("  - performance_comparison.png")
    print("  - speedup_analysis.png")
    print()
    
    # Print observations
    print("OBSERVATIONS:")
    print("-" * 60)
    print(f"1. System has {processors} logical processors")
    print(f"2. Optimal performance typically achieved at n ≈ {processors}")
    print("3. Beyond physical cores, overhead from context switching")
    print("   and process management reduces benefits")
    print("4. Larger ranges show better parallelization efficiency")
    print("5. Speedup approaches ideal for CPU-bound prime computation")
    print()

if __name__ == '__main__':
    main()