using Microsoft.UI.Dispatching;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using Chess_UI.Services;
using System.Collections.ObjectModel;


namespace Chess_UI.ViewModels
{
    public class SettingsViewModel : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        private readonly DispatcherQueue DispatcherQueue;

        private ThemeLoader ThemeLoader;

        private readonly ThemeManager themeManager;

        public ObservableCollection<BoardTheme> BoardThemes { get; }

        public ObservableCollection<PieceTheme> PieceThemes { get; }


        public SettingsViewModel(DispatcherQueue dispatcherQueue, ThemeManager themeManager)
        {
            this.DispatcherQueue = dispatcherQueue;
            ThemeLoader = new();
            this.themeManager = themeManager;

            BoardThemes = new ObservableCollection<BoardTheme>(ThemeLoader.LoadBoardThemes());
            PieceThemes = new ObservableCollection<PieceTheme>(ThemeLoader.LoadPieceThemes());

            SelectedBoardTheme = GetCurrentSelectedBoardTheme();
            SelectedPieceTheme = GetCurrentSelectedPieceTheme();
        }


        private BoardTheme GetCurrentSelectedBoardTheme()
        {
            string currentThemeName = Configuration.CurrentBoardTheme;
            BoardTheme theme = BoardThemes.FirstOrDefault(b => string.Equals(b.Name, currentThemeName, StringComparison.OrdinalIgnoreCase));
            return theme;
        }


        private PieceTheme GetCurrentSelectedPieceTheme()
        {
            string currentThemeName = Configuration.CurrentPieceTheme;
            PieceTheme theme = PieceThemes.FirstOrDefault(p => string.Equals(p.Name, currentThemeName, StringComparison.OrdinalIgnoreCase));
            return theme;
        }


        private BoardTheme selectedBoardTheme;
        public BoardTheme SelectedBoardTheme
        {
            get => selectedBoardTheme;
            set
            {
                if (selectedBoardTheme != value)
                {
                    selectedBoardTheme = value;
                    if (value != null)
                        Configuration.CurrentBoardTheme = value.Name;
                    OnPropertyChanged();

                    // Update ThemeManager’s board theme
                    // This triggers property change events in the manager
                    themeManager.CurrentBoardTheme = value;
                }
            }
        }


        private PieceTheme selectedPieceTheme;
        public PieceTheme SelectedPieceTheme
        {
            get => selectedPieceTheme;
            set
            {
                if (selectedPieceTheme != value)
                {
                    selectedPieceTheme = value;
                    if (value != null)
                        Configuration.CurrentPieceTheme = value.Name;
                    OnPropertyChanged();

                    // Update ThemeManager’s piece theme
                    themeManager.CurrentPieceTheme = value;
                }
            }
        }


        protected void OnPropertyChanged([CallerMemberName] string name = null)
        {
            DispatcherQueue.TryEnqueue(() =>
            {
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
            });
        }
    }
}
