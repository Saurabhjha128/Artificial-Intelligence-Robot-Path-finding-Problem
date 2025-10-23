import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

def plot_csp_performance():
    # Read the performance data
    df = pd.read_csv('performance_data.csv')
    
    # Filter only successful runs
    successful_runs = df[df['SolutionFound'] == 'Yes']
    
    # Create subplots
    fig, axes = plt.subplots(2, 2, figsize=(15, 12))
    
    # Plot 1: Average Time Taken
    time_data = successful_runs.groupby('Method')['TimeTaken'].mean().reset_index()
    sns.barplot(data=time_data, x='Method', y='TimeTaken', ax=axes[0,0])
    axes[0,0].set_title('Average Time Taken (seconds)')
    axes[0,0].set_ylabel('Time (s)')
    
    # Plot 2: Average Backtrack Count
    backtrack_data = successful_runs.groupby('Method')['BacktrackCount'].mean().reset_index()
    sns.barplot(data=backtrack_data, x='Method', y='BacktrackCount', ax=axes[0,1])
    axes[0,1].set_title('Average Backtrack Count')
    axes[0,1].set_ylabel('Backtracks')
    
    # Plot 3: Average Assignments
    assign_data = successful_runs.groupby('Method')['Assignments'].mean().reset_index()
    sns.barplot(data=assign_data, x='Method', y='Assignments', ax=axes[1,0])
    axes[1,0].set_title('Average Assignment Count')
    axes[1,0].set_ylabel('Assignments')
    
    # Plot 4: Success Rate
    success_rate = df.groupby('Method')['SolutionFound'].apply(
        lambda x: (x == 'Yes').sum() / len(x) * 100
    ).reset_index()
    sns.barplot(data=success_rate, x='Method', y='SolutionFound', ax=axes[1,1])
    axes[1,1].set_title('Success Rate (%)')
    axes[1,1].set_ylabel('Success Rate (%)')
    
    plt.tight_layout()
    plt.savefig('csp_performance_comparison.png', dpi=300, bbox_inches='tight')
    plt.show()
    
    # Print summary statistics
    print("CSP Performance Summary:")
    print("=" * 50)
    for method in df['Method'].unique():
        method_data = df[df['Method'] == method]
        success_data = method_data[method_data['SolutionFound'] == 'Yes']
        
        print(f"\n{method}:")
        print(f"  Success Rate: {(len(success_data)/len(method_data)*100):.2f}%")
        if len(success_data) > 0:
            print(f"  Avg Time: {success_data['TimeTaken'].mean():.6f}s")
            print(f"  Avg Backtracks: {success_data['BacktrackCount'].mean():.2f}")
            print(f"  Avg Assignments: {success_data['Assignments'].mean():.2f}")

if __name__ == "__main__":
    plot_csp_performance()