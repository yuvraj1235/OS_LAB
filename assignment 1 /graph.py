import pandas as pd
import matplotlib.pyplot as plt
import os

def plot_ranges():
    # Path to the data file
    data_path = 'human/results.csv'
    output_plot = 'human/range_comparison_plot.png'

    if not os.path.exists(data_path):
        print(f"Error: {data_path} not found. Run the C program first!")
        return

    df = pd.read_csv(data_path)
    plt.figure(figsize=(10, 6))

    for r in df['Range'].unique():
        subset = df[df['Range'] == r]
        plt.plot(subset['Processes'], subset['Time'], marker='o', label=f'Range {r}')

    plt.title('Execution Time vs. Number of Processes (n)')
    plt.xlabel('Number of Processes (n)')
    plt.ylabel('Execution Time (seconds)')
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.legend()
    
    # Save the graph inside the man_made folder
    plt.savefig(output_plot)
    print(f"Graph successfully stored in: {output_plot}")
    plt.show()

if __name__ == "__main__":
    plot_ranges()