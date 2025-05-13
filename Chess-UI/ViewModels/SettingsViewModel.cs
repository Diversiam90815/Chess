using Microsoft.UI.Dispatching;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Collections.ObjectModel;
using Chess_UI.Themes;
using Chess_UI.Settings;


namespace Chess_UI.ViewModels
{
    public class SettingsViewModel : ISettings
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


        public BoardTheme GetCurrentSelectedBoardTheme()
        {
            string currentThemeName = Settings.Settings.CurrentBoardTheme;
            BoardTheme theme = BoardThemes.FirstOrDefault(b => string.Equals(b.Name, currentThemeName, StringComparison.OrdinalIgnoreCase));
            return theme;
        }


        public PieceTheme GetCurrentSelectedPieceTheme()
        {
            string currentThemeName = Settings.Settings.CurrentPieceTheme;
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
                        Settings.Settings.CurrentBoardTheme = value.Name;

                    OnPropertyChanged();

                    // Update ThemeManager’s board theme
                    // This triggers property change events in the manager
                    themeManager.CurrentBoardTheme = value.BoardThemeID;
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
                        Settings.Settings.CurrentPieceTheme = value.Name;
                    OnPropertyChanged();

                    // Update ThemeManager’s piece theme
                    themeManager.CurrentPieceTheme = value.PieceThemeID;
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
