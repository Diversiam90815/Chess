#!/usr/bin/env python3

'''
Chess Engine Performance Analysis Pipeline

This master script runs both the data collector and analyzer in a single flow
'''

import os
import sys
import argparse
from datetime import datetime
from pathlib import Path

from performance_data_collector import PerformanceDataCollector
from performance_analyzer import ChessEngineAnalyzer


class PerformancePipeline:
    '''Orchestrating the complete performance pipeline'''

    def __init__(self, data_dir: str = None, output_dir: str = None, no_html: bool = False):
        '''
        Initialization of the pipeline
        
        Args:
            data_dir: Dirctory to search for the performance JSON files
            output_dir: Directory to save analysis outputs
            not_html: Skip HTML output generation        
        '''

        self.data_dir = data_dir
        self.output_dir = output_dir or f"./performance_analysis_{datetime.now().strftime('%Y%m%d_%H%M%S')}"
        self.no_html = no_html
        self.data_file = os.path.join(self.output_dir, "chess_engine_performance_data.json")


    def run_collection(self):
        '''Run data collection phase'''

        print("=" * 60)
        print("PHASE 1 : DATA COLLECTION")
        print("=" * 60)

        try:
            # Initialize Collector
            collector = PerformanceDataCollector(self.data_dir)

            # Collect all data
            print("Collecting chess performance data...")
            collection = collector.collect_all_data()

            if not collection.all_data:
                print("No performance data found!")
                return False
            
            # Print statistics
            stats = collector.get_statistics()
            print("\n=== Collection Statistics ===")
            print(f" Total files processed: {stats['total_files']}")
            print(f" Total test results: {stats['total_test_results']}")
            print(f" App versions found: {stats['versions']}")
            print(f" Version distribution: {stats['version_counts']}")
            print(f" Date range: {stats['date_range']['earliest']} to {stats['date_range']['latest']}")
            print(f" Test groups: {stats['test_groups']}")

            # Create output directory
            os.makedirs(self.output_dir, exist_ok=True)

            # Export all data
            collector.export_to_json(self.data_file)
            print(f"Data exported to {self.data_file}")

            return True
        
        except Exception as ex:
            print(f'Error occured during data collection: {ex}')
            return False
        


    def run_analysis(self) -> bool:
        '''Run the data analyis phase'''

        print("=" * 60)
        print("PHASE 2 : DATA ANALYSIS")
        print("=" * 60)

        try:
            # Check if data file exists
            if not os.path.exists(self.data_file):
                print(f'Data file not found: {self.data_file}')
                return False
            
            # Change to output directory
            original_dir = os.getcwd()
            os.chdir(self.output_dir)

            try:
                # Initialize analyzer
                analyzer = ChessEngineAnalyzer(os.path.basename(self.data_file))

                # Generate overview report
                print("\nGenerating overview report..")
                analyzer.generate_overview_report()

                # Create visualizations
                print("\nCreating visualizations")
                save_html = not self.no_html

                try:
                    analyzer.plot_performance_trends(save_html=save_html)
                    print("Performance trends created")
                    
                    analyzer.plot_version_comparison(save_html=save_html)
                    print("Version comparison created")
                    
                    analyzer.plot_test_type_analysis(save_html=save_html)
                    print("Test type analysis created")
                    
                    analyzer.create_performance_dashboard(save_html=save_html)
                    print("Performance dashboard created")
                    
                except Exception as e:
                    print(f"Some visualizations failed: {e}")

                # Generate reports
                print("\nGenerating reports...")
                analyzer.generate_performance_report()
                print("Performance report generated")
                
                analyzer.export_summary_csv()
                print("Summary CSV exported")
                
                return True
                
            finally:
                # Restore original directory
                os.chdir(original_dir)

        except Exception as ex:
            print(f"Error occured during analysis: {ex}")
            return False
        

    def run_pipeline(self) -> bool:
        '''Run the complete pipeline'''

        print("Starting Chess Engine Performance Analysis pipeline")
        print(f"Data directory: {self.data_dir or 'Default Search paths'}")
        print(f"Output Directory: {self.output_dir}")
        print(f"HTML output: {'Disabled' if self.no_html else 'Enabled'}")

        # Phase 1: Collection
        if not self.run_collection():
            print("\nPipeline failed during data collection phase")
            return False
        
        # Phase 2: Analysis
        if not self.run_analysis():
            print("\nPipeline failed during data analysis phase")
            return False
        
        # Success
        print("\n" + "=" * 60)
        print("PIPELINE COMPLETED SUCCESSFULLY!")
        print("=" * 60)
        print(f"Results saved to: {os.path.abspath(self.output_dir)}")
        print("\nGenerated files:")
        
        files = [
            ("chess_engine_performance_data.json", "Raw collected data"),
            ("performance_report.txt", "Detailed analysis report"),
            ("performance_summary.csv", "Summary statistics"),
            ("test_type_analysis.png", "Test type analysis chart")
        ]
        
        if not self.no_html:
            files.extend([
                ("performance_trends.html", "Interactive performance trends"),
                ("version_comparison.html", "Interactive version comparison"),
                ("performance_dashboard.html", "Interactive dashboard")
            ])

        for filename, description in files:
            filepath = os.path.join(self.output_dir, filename)
            
            if os.path.exists(filepath):
                print(f"{filename} - {description}")
            else:
                print(f"{filename} - {description} - not generated")

        return True


def main():
    '''Main function with command line arguments'''
    parser = argparse.ArgumentParser( 
        description='Complete Chess Engine Performance Analysis Pipeline',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=
        """
                Examples:
                python chess_performance_pipeline.py
                python chess_performance_pipeline.py --data-dir ./performance_data
                python chess_performance_pipeline.py --output-dir ./my_analysis --no-html
        """)
    
    parser.add_argument('--data-dir', 
                       help='Directory to search for performance JSON files (default: auto-detect)')
    parser.add_argument('--output-dir', 
                       help='Directory to save analysis outputs (default: timestamped folder)')
    parser.add_argument('--no-html', action='store_true',
                       help='Skip HTML output generation')
    parser.add_argument('--collect-only', action='store_true',
                       help='Only run data collection phase')
    parser.add_argument('--analyze-only', action='store_true',
                       help='Only run analysis phase (requires existing data file)')  

    args = parser.parse_args()
    
    # Validate arguments
    if args.collect_only and args.analyze_only:
        print("Cannot specify both --collect-only and --analyze-only")
        sys.exit(1)

    # Initialize pipeline
    pipeline = PerformancePipeline(data_dir=args.data_dir, output_dir=args.output_dir, no_html=args.no_html)
    
    # Run requested phases
    success = False

    if args.analyze_only:
        # Check if data file exists
        if args.output_dir and os.path.exists(os.path.join(args.output_dir, "chess_engine_performance_data.json")):
            pipeline.data_file = os.path.join(args.output_dir, "chess_engine_performance_data.json")
            success = pipeline.run_analysis()
        else:
            print("No existing data file found. Run collection first or provide correct output directory")
            sys.exit(1)
    elif args.collect_only:
        success = pipeline.run_collection()
    else:
        # Run complete pipeline
        success = pipeline.run_pipeline()

    # Exit with appropriate code
    sys.exit(0 if success else 1)



if __name__ == "__main__":
    main()        
