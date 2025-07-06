using System;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Collections.ObjectModel;
using Chess.UI.Themes;
using Chess.UI.Wrappers;
using Chess.UI.Themes.Interfaces;


namespace Chess.UI.ViewModels
{
    public class ThemePreferencesViewModel
    {
        public event PropertyChangedEventHandler PropertyChanged;

        private readonly IDispatcherQueueWrapper _dispatcherQueueWrapper;

        private readonly ThemeLoader _themeLoader;

        private readonly IThemeManager _themeManager;

        public ObservableCollection<BoardTheme> BoardThemes { get; }

        public ObservableCollection<PieceTheme> PieceThemes { get; }


        public ThemePreferencesViewModel(IDispatcherQueueWrapper dispatcherQueue, IThemeManager themeManager)
        {
            _dispatcherQueueWrapper = dispatcherQueue;
            _themeLoader = new();
            _themeManager = themeManager;

            BoardThemes = new ObservableCollection<BoardTheme>(_themeLoader.LoadBoardThemes());
            PieceThemes = new ObservableCollection<PieceTheme>(_themeLoader.LoadPieceThemes());

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


        private BoardTheme _selectedBoardTheme;
        public BoardTheme SelectedBoardTheme
        {
            get => _selectedBoardTheme;
            set
            {
                if (_selectedBoardTheme != value)
                {
                    _selectedBoardTheme = value;

                    if (value != null)
                        Settings.Settings.CurrentBoardTheme = value.Name;

                    OnPropertyChanged();

                    // Update ThemeManager’s board theme
                    // This triggers property change events in the manager
                    _themeManager.CurrentBoardTheme = value.BoardThemeID;
                }
            }
        }


        private PieceTheme _selectedPieceTheme;
        public PieceTheme SelectedPieceTheme
        {
            get => _selectedPieceTheme;
            set
            {
                if (_selectedPieceTheme != value)
                {
                    _selectedPieceTheme = value;
                    if (value != null)
                        Settings.Settings.CurrentPieceTheme = value.Name;
                    OnPropertyChanged();

                    // Update ThemeManager’s piece theme
                    _themeManager.CurrentPieceTheme = value.PieceThemeID;
                }
            }
        }


        protected void OnPropertyChanged([CallerMemberName] string name = null)
        {
            _dispatcherQueueWrapper.TryEnqueue(() =>
            {
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
            });
        }
    }
}
