import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

def plot_performance_comparison():
    # Read the performance data
    df = pd.read_csv('performance_data.csv')
    
    # Filter only successful runs for fair comparison
    successful_runs = df[df['PathFound'] == 'Yes']
    
    # Create subplots
    fig, axes = plt.subplots(2, 2, figsize=(15, 12))
    
    # Plot 1: Average Time Taken
    time_data = successful_runs.groupby('Heuristic')['TimeTaken'].mean().reset_index()
    sns.barplot(data=time_data, x='Heuristic', y='TimeTaken', ax=axes[0,0])
    axes[0,0].set_title('Average Time Taken (seconds)')
    axes[0,0].set_ylabel('Time (s)')
    
    # Plot 2: Average Path Length
    path_data = successful_runs.groupby('Heuristic')['PathLength'].mean().reset_index()
    sns.barplot(data=path_data, x='Heuristic', y='PathLength', ax=axes[0,1])
    axes[0,1].set_title('Average Path Length')
    axes[0,1].set_ylabel('Path Length')
    
    # Plot 3: Average Nodes Expanded
    nodes_data = successful_runs.groupby('Heuristic')['NodesExpanded'].mean().reset_index()
    sns.barplot(data=nodes_data, x='Heuristic', y='NodesExpanded', ax=axes[1,0])
    axes[1,0].set_title('Average Nodes Expanded')
    axes[1,0].set_ylabel('Nodes Expanded')
    
    # Plot 4: Success Rate
    success_rate = df.groupby('Heuristic')['PathFound'].apply(
        lambda x: (x == 'Yes').sum() / len(x) * 100
    ).reset_index()
    sns.barplot(data=success_rate, x='Heuristic', y='PathFound', ax=axes[1,1])
    axes[1,1].set_title('Success Rate (%)')
    axes[1,1].set_ylabel('Success Rate (%)')
    
    plt.tight_layout()
    plt.savefig('performance_comparison.png', dpi=300, bbox_inches='tight')
    plt.show()
    
    # Print summary statistics
    print("Performance Summary:")
    print("=" * 50)
    for heuristic in df['Heuristic'].unique():
        heuristic_data = df[df['Heuristic'] == heuristic]
        success_data = heuristic_data[heuristic_data['PathFound'] == 'Yes']
        
        print(f"\n{heuristic} Heuristic:")
        print(f"  Success Rate: {(len(success_data)/len(heuristic_data)*100):.2f}%")
        if len(success_data) > 0:
            print(f"  Avg Time: {success_data['TimeTaken'].mean():.6f}s")
            print(f"  Avg Path Length: {success_data['PathLength'].mean():.2f}")
            print(f"  Avg Nodes Expanded: {success_data['NodesExpanded'].mean():.2f}")

if __name__ == "__main__":
    plot_performance_comparison()