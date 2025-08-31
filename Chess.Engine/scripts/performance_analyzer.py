#!/usr/bin/env python3

'''
Chess Engine Performance Data Analyzer and Visualizer

This script analyzes and visualizes the collected performance data from the Chess Engine
performance tests, providing insights into performance trends, comparisons and metrics.
'''


import os
import json
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
from datetime import datetime, timedelta
import plotly.graph_objects as go
import plotly.express as px
from plotly.subplots import make_subplots
import plotly.offline as pyo
from typing import Dict, List, Any, Optional
import argparse


# Set style for better looking plots
plt.style.use('seaborn-v0_8')
sns.set_palette("husl")


class ChessEngineAnalyzer:
    '''Analyzes and visualizes Chess Engine Performance Data'''

    def __init__(self, data_file: str = "chess_engine_performance_data.json"):
        '''Initializes the analyzer with collected data

        Args:
            data_file : Path to the collected performance data JSON file
        '''

        self.data_file = data_file
        self.data = None
        self.df = None
        self.load_data()


    def load_data(self):
        '''Load performance data from JSON file'''
        try:
            with open(self.data_file, 'r', encoding='utf-8') as f:
                self.data = json.load(f)

            # Convert to DataFrame for easier analysis
            self.df = self._create_dataframe()
            print(f'Loaded {len(self.df)} performance test results')

        except FileNotFoundError:
            print(f'Data file {self.data_file} not found. Please run the collector script first and set the correct path')
            exit(1)
        except Exception as ex:
            print(f'Error loading data: {ex}')
            exit(1)


    def _create_dataframe(self) -> pd.DataFrame:
        '''Convert JSON data to pandas DataFrame'''
        rows = []

        for entry in self.data['data']:
            for result in entry['results']:
                row = {
                    'app_version': entry['app_version'],
                    'timestamp': pd.to_datetime(entry['timestamp']),
                    'test_group': entry['test_group'],
                    'test_file': entry['test_file'],
                    'file_path': entry['file_path'],
                    'test_name': result.get('testName', 'Unknown'),
                }

                # Add performance metrics based on test type
                if 'performance' in result:
                    perf = result['performance']
                    row.update({
                        'duration_microseconds': perf.get('durationMicroseconds', 0),
                        'duration_seconds': perf.get('durationMicroseconds', 0) / 1_000_000,
                    })
                    
                    # Move generation specific metrics
                    if 'movesGenerated' in perf:
                        row.update({
                            'moves_generated': perf.get('movesGenerated', 0),
                            'moves_per_second': perf.get('movesPerSecond', 0),
                            'positions_evaluated': perf.get('positionsEvaluated', 0),
                        })
                    
                    # Evaluation specific metrics
                    if 'evaluationsPerformed' in perf:
                        row.update({
                            'evaluations_performed': perf.get('evaluationsPerformed', 0),
                            'evaluations_per_second': perf.get('evaluationsPerSecond', 0),
                            'average_evaluation_time': perf.get('averageEvaluationTimeMicroseconds', 0),
                        })
                    
                    # Operations specific metrics (for LightChessBoard)
                    if 'operationsPerformed' in perf:
                        row.update({
                            'operations_performed': perf.get('operationsPerformed', 0),
                            'operations_per_second': perf.get('operationsPerSecond', 0),
                            'average_operation_time': perf.get('averageOperationTimeMicroseconds', 0),
                        })

                # Add board configuration if available
                if 'boardConfiguration' in result:
                    row['board_configuration'] = result['boardConfiguration']
                if 'operation' in result:
                    row['operation'] = result['operation']
                if 'evaluationType' in result:
                    row['evaluation_type'] = result['evaluationType']
                
                # Add score information if available
                if 'scores' in result:
                    scores = result['scores']
                    row.update({
                        'min_score': scores.get('minimum', 0),
                        'max_score': scores.get('maximum', 0),
                        'average_score': scores.get('average', 0),
                    })
                
                rows.append(row)

        return pd.DataFrame(rows)


    def generate_overview_report(self):
        '''Generate an overview report of the performance data'''
        print("=" * 60)
        print("CHESS ENGINE PERFORMANCE ANALYSIS OVERVIEW")
        print("=" * 60)

        print(f'Analysis period: {self.df['timestamp'].min()} - {self.df['timestamp'].max()}')
        print(f'Total test results: {len(self.df)}')
        print(f'App versions: {', '.join(self.df['test_group'].unique())}')

        print("\n" + "-" * 40)
        print("TEST DISTRIBUTION BY GROUP:")
        print(self.df['test_group'].value_counts().to_string())

        if 'test_name' in self.df.columns:
            print("\n" + "-" * 40)
            print("TEST DISTRIBUTION BY TYPE:")
            print(self.df['test_name'].value_counts().to_string())

        print("\n" + "-" * 40)
        print("PERFORMANCE METRICS SUMMARY:")

        numeric_cols = self.df.select_dtypes(include=[np.number]).columns
        if len(numeric_cols) > 0:
            print(self.df[numeric_cols].describe())


    def plot_performance_trends(self, save_html: bool = True):
        '''Create interactive performance trend plots'''

        # Create subplotsfor different metrics
        fig = make_subplots(
            rows=2, cols=2,
            subplot_titles=('Move Generation Performance', 'Evaluation Performance', 
                            'Duration Trends', 'Operations Performance'),
            specs=[[{"secondary_y": True}, {"secondary_y": True}],
                    [{"secondary_y": False}, {"secondary_y": True}]]
        )
        
        # Plot 1: Move Generation Performance
        move_gen_data = self.df[self.df['test_group'].str.contains('Move Generation', na=False)]
        if not move_gen_data.empty and 'moves_per_second' in move_gen_data.columns:
            for version in move_gen_data['app_version'].unique():
                version_data = move_gen_data[move_gen_data['app_version'] == version]
                fig.add_trace(
                    go.Scatter(x=version_data['timestamp'], y=version_data['moves_per_second'],
                            mode='lines+markers', name=f'Moves/sec v{version}'),
                    row=1, col=1
                )
        
        # Plot 2: Evaluation Performance
        eval_data = self.df[self.df['test_group'].str.contains('Evaluation', na=False)]
        if not eval_data.empty and 'evaluations_per_second' in eval_data.columns:
            for version in eval_data['app_version'].unique():
                version_data = eval_data[eval_data['app_version'] == version]
                fig.add_trace(
                    go.Scatter(x=version_data['timestamp'], y=version_data['evaluations_per_second'],
                            mode='lines+markers', name=f'Eval/sec v{version}'),
                    row=1, col=2
                )
        
        # Plot 3: Duration Trends
        if 'duration_seconds' in self.df.columns:
            for test_group in self.df['test_group'].unique():
                group_data = self.df[self.df['test_group'] == test_group]
                fig.add_trace(
                    go.Scatter(x=group_data['timestamp'], y=group_data['duration_seconds'],
                            mode='lines+markers', name=f'Duration {test_group}'),
                    row=2, col=1
                )
        
        # Plot 4: Operations Performance
        ops_data = self.df[self.df['test_group'].str.contains('Board', na=False)]
        if not ops_data.empty and 'operations_per_second' in ops_data.columns:
            for version in ops_data['app_version'].unique():
                version_data = ops_data[ops_data['app_version'] == version]
                fig.add_trace(
                    go.Scatter(x=version_data['timestamp'], y=version_data['operations_per_second'],
                            mode='lines+markers', name=f'Ops/sec v{version}'),
                    row=2, col=2
                )
        
        fig.update_layout(
            title="Chess Engine Performance Trends Over Time",
            height=800,
            showlegend=True
        )
        
        fig.update_xaxes(title_text="Time")
        fig.update_yaxes(title_text="Moves/Second", row=1, col=1)
        fig.update_yaxes(title_text="Evaluations/Second", row=1, col=2)
        fig.update_yaxes(title_text="Duration (seconds)", row=2, col=1)
        fig.update_yaxes(title_text="Operations/Second", row=2, col=2)
        
        if save_html:
            fig.write_html("performance_trends.html")
            print("Performance trends saved to performance_trends.html")
        
        fig.show()
        
        return fig


    def plot_version_comparison(self, save_html: bool = True):
        '''Compare performance across different app versions'''
       
        if len(self.df['app_version'].unique()) < 2:
            print("Need at least 2 versions to compare")
            return
        
        fig = make_subplots(
            rows=2, cols=2,
            subplot_titles=('Moves Per Second by Version', 'Average Duration by Version',
                          'Evaluations Per Second by Version', 'Performance Distribution'),
            specs=[[{"type": "box"}, {"type": "box"}],
                   [{"type": "box"}, {"type": "violin"}]]
        )
        
        # Box plot for moves per second
        if 'moves_per_second' in self.df.columns:
            for version in self.df['app_version'].unique():
                version_data = self.df[self.df['app_version'] == version]
                moves_data = version_data['moves_per_second'].dropna()
                if not moves_data.empty:
                    fig.add_trace(
                        go.Box(y=moves_data, name=f'v{version}', boxpoints='all'),
                        row=1, col=1
                    )
        
        # Box plot for duration
        if 'duration_seconds' in self.df.columns:
            for version in self.df['app_version'].unique():
                version_data = self.df[self.df['app_version'] == version]
                duration_data = version_data['duration_seconds'].dropna()
                if not duration_data.empty:
                    fig.add_trace(
                        go.Box(y=duration_data, name=f'v{version}', boxpoints='all'),
                        row=1, col=2
                    )
        
        # Box plot for evaluations per second
        if 'evaluations_per_second' in self.df.columns:
            for version in self.df['app_version'].unique():
                version_data = self.df[self.df['app_version'] == version]
                eval_data = version_data['evaluations_per_second'].dropna()
                if not eval_data.empty:
                    fig.add_trace(
                        go.Box(y=eval_data, name=f'v{version}', boxpoints='all'),
                        row=2, col=1
                    )
        
        # Violin plot for overall performance distribution
        if 'moves_per_second' in self.df.columns:
            for version in self.df['app_version'].unique():
                version_data = self.df[self.df['app_version'] == version]
                moves_data = version_data['moves_per_second'].dropna()
                if not moves_data.empty:
                    fig.add_trace(
                        go.Violin(y=moves_data, name=f'v{version}', box_visible=True),
                        row=2, col=2
                    )
        
        fig.update_layout(
            title="Performance Comparison Across Versions",
            height=800,
            showlegend=False
        )
        
        if save_html:
            fig.write_html("version_comparison.html")
            print("Version comparison saved to version_comparison.html")
        
        fig.show()
        
        return fig
    

    def plot_test_type_analysis(self):
        '''Analyze performance by test type'''

        # Create heatmap of performance metrics by test type
        plt.figure(figsize=(15, 10))
        
        # Prepare data for heatmap
        numeric_cols = ['duration_seconds', 'moves_per_second', 'evaluations_per_second', 'operations_per_second']
        available_cols = [col for col in numeric_cols if col in self.df.columns]
        
        if available_cols and 'test_name' in self.df.columns:
            heatmap_data = self.df.groupby('test_name')[available_cols].mean()
            
            plt.subplot(2, 2, 1)
            sns.heatmap(heatmap_data.T, annot=True, fmt='.2f', cmap='YlOrRd', cbar_kws={'label': 'Average Value'})
            plt.title('Performance Metrics by Test Type')
            plt.xlabel('Test Type')
            plt.ylabel('Metrics')
        
        # Performance distribution by board configuration
        if 'board_configuration' in self.df.columns and 'moves_per_second' in self.df.columns:
            plt.subplot(2, 2, 2)
            board_data = self.df.groupby('board_configuration')['moves_per_second'].mean().sort_values(ascending=False)
            board_data.plot(kind='bar')
            plt.title('Moves Per Second by Board Configuration')
            plt.xlabel('Board Configuration')
            plt.ylabel('Moves/Second')
            plt.xticks(rotation=45)
        
        # Duration comparison
        if 'test_name' in self.df.columns and 'duration_seconds' in self.df.columns:
            plt.subplot(2, 2, 3)
            test_duration = self.df.groupby('test_name')['duration_seconds'].mean().sort_values(ascending=False)
            test_duration.plot(kind='bar', color='skyblue')
            plt.title('Average Test Duration by Type')
            plt.xlabel('Test Type')
            plt.ylabel('Duration (seconds)')
            plt.xticks(rotation=45)
        
        # Performance consistency (coefficient of variation)
        if 'test_name' in self.df.columns and 'moves_per_second' in self.df.columns:
            plt.subplot(2, 2, 4)
            consistency = self.df.groupby('test_name')['moves_per_second'].agg(['mean', 'std'])
            consistency['cv'] = consistency['std'] / consistency['mean']
            consistency['cv'].sort_values().plot(kind='bar', color='lightcoral')
            plt.title('Performance Consistency (Lower = More Consistent)')
            plt.xlabel('Test Type')
            plt.ylabel('Coefficient of Variation')
            plt.xticks(rotation=45)
        
        plt.tight_layout()
        plt.savefig('test_type_analysis.png', dpi=300, bbox_inches='tight')
        print("Test type analysis saved to test_type_analysis.png")
        plt.show()


    def create_performance_dashboard(self, save_html: bool = True):
        '''Create a comprehensive performance dashboard'''
       
        # Create a dashboard with multiple charts
        fig = make_subplots(
            rows=3, cols=2,
            subplot_titles=(
                'Performance Over Time', 'Version Comparison',
                'Test Type Distribution', 'Duration Analysis',
                'Top Performing Configurations', 'Performance Correlation Matrix'
            ),
            specs=[
                [{"secondary_y": True}, {"type": "box"}],
                [{"type": "pie"}, {"type": "bar"}],
                [{"type": "bar"}, {"type": "heatmap"}]
            ]
        )
        
        # 1. Performance over time
        if 'moves_per_second' in self.df.columns:
            daily_perf = self.df.groupby(self.df['timestamp'].dt.date)['moves_per_second'].mean()
            fig.add_trace(
                go.Scatter(x=daily_perf.index, y=daily_perf.values,
                         mode='lines+markers', name='Daily Avg Moves/sec'),
                row=1, col=1
            )
        
        # 2. Version comparison box plot
        if 'moves_per_second' in self.df.columns:
            for version in self.df['app_version'].unique():
                version_data = self.df[self.df['app_version'] == version]['moves_per_second'].dropna()
                if not version_data.empty:
                    fig.add_trace(
                        go.Box(y=version_data, name=f'v{version}'),
                        row=1, col=2
                    )
        
        # 3. Test type distribution
        if 'test_name' in self.df.columns:
            test_counts = self.df['test_name'].value_counts()
            fig.add_trace(
                go.Pie(labels=test_counts.index, values=test_counts.values, name="Test Distribution"),
                row=2, col=1
            )
        
        # 4. Duration analysis
        if 'duration_seconds' in self.df.columns and 'test_name' in self.df.columns:
            duration_avg = self.df.groupby('test_name')['duration_seconds'].mean()
            fig.add_trace(
                go.Bar(x=duration_avg.index, y=duration_avg.values, name="Avg Duration"),
                row=2, col=2
            )
        
        # 5. Top performing configurations
        if 'board_configuration' in self.df.columns and 'moves_per_second' in self.df.columns:
            config_perf = self.df.groupby('board_configuration')['moves_per_second'].mean().nlargest(5)
            fig.add_trace(
                go.Bar(x=config_perf.values, y=config_perf.index, orientation='h', name="Top Configs"),
                row=3, col=1
            )
        
        # 6. Correlation matrix (if enough numeric data)
        numeric_cols = self.df.select_dtypes(include=[np.number]).columns[:5]  # Limit to 5 for readability
        if len(numeric_cols) > 1:
            corr_matrix = self.df[numeric_cols].corr()
            fig.add_trace(
                go.Heatmap(z=corr_matrix.values, x=corr_matrix.columns, y=corr_matrix.columns,
                          colorscale='RdBu', zmid=0),
                row=3, col=2
            )
        
        fig.update_layout(
            title="Chess Engine Performance Dashboard",
            height=1200,
            showlegend=True
        )
        
        if save_html:
            fig.write_html("performance_dashboard.html")
            print("Performance dashboard saved to performance_dashboard.html")
        
        fig.show()
        
        return fig

   
    def generate_performance_report(self, output_file: str = "performance_report.txt"):
        '''Generate a detailed text report'''
      
        with open(output_file, 'w') as f:
            f.write("CHESS ENGINE PERFORMANCE ANALYSIS REPORT\n")
            f.write("=" * 50 + "\n\n")
            f.write(f"Report Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
            f.write(f"Data Period: {self.df['timestamp'].min()} to {self.df['timestamp'].max()}\n")
            f.write(f"Total Test Results Analyzed: {len(self.df)}\n\n")
            
            # Version analysis
            f.write("VERSION ANALYSIS:\n")
            f.write("-" * 20 + "\n")
            for version in self.df['app_version'].unique():
                version_data = self.df[self.df['app_version'] == version]
                f.write(f"Version {version}: {len(version_data)} tests\n")
                
                if 'moves_per_second' in version_data.columns:
                    moves_stats = version_data['moves_per_second'].describe()
                    f.write(f"  Moves/sec - Mean: {moves_stats['mean']:.2f}, Std: {moves_stats['std']:.2f}\n")
                
                if 'duration_seconds' in version_data.columns:
                    duration_stats = version_data['duration_seconds'].describe()
                    f.write(f"  Duration - Mean: {duration_stats['mean']:.4f}s, Std: {duration_stats['std']:.4f}s\n")
            
            f.write("\n")
            
            # Test type analysis
            if 'test_name' in self.df.columns:
                f.write("TEST TYPE ANALYSIS:\n")
                f.write("-" * 20 + "\n")
                for test_type in self.df['test_name'].unique():
                    test_data = self.df[self.df['test_name'] == test_type]
                    f.write(f"{test_type}: {len(test_data)} tests\n")
                    
                    if 'moves_per_second' in test_data.columns:
                        moves_mean = test_data['moves_per_second'].mean()
                        f.write(f"  Average Moves/sec: {moves_mean:.2f}\n")
                    
                    if 'duration_seconds' in test_data.columns:
                        duration_mean = test_data['duration_seconds'].mean()
                        f.write(f"  Average Duration: {duration_mean:.4f}s\n")
                
                f.write("\n")
            
            # Performance recommendations
            f.write("PERFORMANCE INSIGHTS & RECOMMENDATIONS:\n")
            f.write("-" * 40 + "\n")
            
            if 'moves_per_second' in self.df.columns:
                best_performance = self.df.loc[self.df['moves_per_second'].idxmax()]
                f.write(f"Best performance: {best_performance['moves_per_second']:.2f} moves/sec\n")
                f.write(f"  Test: {best_performance.get('test_name', 'Unknown')}\n")
                f.write(f"  Version: {best_performance['app_version']}\n")
                f.write(f"  Configuration: {best_performance.get('board_configuration', 'Unknown')}\n\n")
            
            # Identify performance trends
            if len(self.df['app_version'].unique()) > 1:
                f.write("VERSION COMPARISON:\n")
                versions_sorted = sorted(self.df['app_version'].unique())
                if len(versions_sorted) >= 2:
                    latest = versions_sorted[-1]
                    previous = versions_sorted[-2]
                    
                    latest_data = self.df[self.df['app_version'] == latest]
                    previous_data = self.df[self.df['app_version'] == previous]
                    
                    if 'moves_per_second' in self.df.columns:
                        latest_avg = latest_data['moves_per_second'].mean()
                        previous_avg = previous_data['moves_per_second'].mean()
                        improvement = ((latest_avg - previous_avg) / previous_avg) * 100
                        f.write(f"Performance change from v{previous} to v{latest}: {improvement:+.2f}%\n")
        
        print(f"Performance report saved to {output_file}")


    def export_summary_csv(self, output_file: str = "performance_summary.csv"):
        '''Export performance summary to CSV'''
       
        summary_data = []
       
        for version in self.df['app_version'].unique():
            version_data = self.df[self.df['app_version'] == version]
            
            summary = {
                'version': version,
                'total_tests': len(version_data),
                'avg_duration': version_data['duration_seconds'].mean() if 'duration_seconds' in version_data.columns else None,
                'avg_moves_per_second': version_data['moves_per_second'].mean() if 'moves_per_second' in version_data.columns else None,
                'avg_evaluations_per_second': version_data['evaluations_per_second'].mean() if 'evaluations_per_second' in version_data.columns else None,
                'first_test_date': version_data['timestamp'].min(),
                'last_test_date': version_data['timestamp'].max(),
            }
            
            summary_data.append(summary)
        
        summary_df = pd.DataFrame(summary_data)
        summary_df.to_csv(output_file, index=False)
        print(f"Performance summary exported to {output_file}")


def main():
    '''Main function with command line arguments'''

    parser = argparse.ArgumentParser(description="Analyze chess engine performance data")
    parser.add_argument('--data-file', default='chess_engine_performance_data.json', help="Path to the collected performance data JSON file")
    parser.add_argument('--output-dir', default='./analysis_output', help="Directory to save analysis outputs")
    parser.add_argument('--no-html', action='store_true', help="Skip HTML output generation")

    args = parser.parse_args()

    # Create output directory
    os.makedirs(args.output_dir, exist_ok=True)
    os.chdir(args.output_dir)

    # Initialize analyzer
    print("Initializing Chess Engine Performance Analyzer...")
    analyzer = ChessEngineAnalyzer(args.data_file)

    # Generate overview report
    analyzer.generate_overview_report()

    # Create visualizations
    print("\nGenerating performance visualizations...")
    
    save_html = not args.no_html

    try:
        analyzer.plot_performance_trends(save_html=save_html)
        analyzer.plot_version_comparison(save_html=save_html)
        analyzer.create_performance_dashboard(save_html=save_html)
        analyzer.plot_test_type_analysis()
    except Exception as ex:
        print(f'Error creating visualizations: {ex}')

    # Generate reports
    print("\nGenerating performance reports...")
    analyzer.generate_performance_report()
    analyzer.export_summary_csv()

    print(f"\nAnalysis complete! Check the '{args.output_dir}' directory for results.")
    print("Generated files:")
    print("- performance_trends.html (if HTML enabled)")
    print("- version_comparison.html (if HTML enabled)")
    print("- performance_dashboard.html (if HTML enabled)")
    print("- test_type_analysis.png")
    print("- performance_report.txt")
    print("- performance_summary.csv")


if __name__ == "__main__":
    main()
