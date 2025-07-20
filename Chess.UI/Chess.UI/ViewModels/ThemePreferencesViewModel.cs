using Chess.UI.Services;
using Chess.UI.Themes;
using Chess.UI.Themes.Interfaces;
using Chess.UI.Wrappers;
using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Threading.Tasks;
using static Chess.UI.Images.ImageServices;


namespace Chess.UI.ViewModels
{
    public class ThemePreferencesViewModel
    {
        public event PropertyChangedEventHandler PropertyChanged;

        private readonly IDispatcherQueueWrapper _dispatcherQueueWrapper;

        private readonly ThemeLoader _themeLoader;

        private readonly IThemeManager _themeManager;

        private bool _isInitialized = false;

        public ObservableCollection<Themes.BoardTheme> BoardThemes { get; }

        public ObservableCollection<Themes.PieceTheme> PieceThemes { get; }


        public ThemePreferencesViewModel(IDispatcherQueueWrapper dispatcherQueue, IThemeManager themeManager)
        {
            _dispatcherQueueWrapper = dispatcherQueue;
            _themeLoader = new();
            _themeManager = themeManager;

            BoardThemes = [];
            PieceThemes = [];

            _ = InitializeAsync();
        }


        private async Task InitializeAsync()
        {
            if (_isInitialized) return;

            try
            {
                var (boardThemes, pieceThemes) = await Task.Run(() =>
                {
                    var boards = _themeLoader.LoadBoardThemes();
                    var pieces = _themeLoader.LoadPieceThemes();
                    return (boards, pieces);
                });

                _dispatcherQueueWrapper.TryEnqueue(() =>
                {
                    foreach (var board in boardThemes)
                        BoardThemes.Add(board);

                    foreach (var piece in pieceThemes)
                        PieceThemes.Add(piece);

                    SelectedBoardTheme = GetCurrentSelectedBoardTheme();
                    SelectedPieceTheme = GetCurrentSelectedPieceTheme();

                    _isInitialized = true;
                });
            }
            catch (Exception ex)
            {
                Logger.LogError($"Failed to initialize themes: {ex.Message}");
            }

        }


        public Themes.BoardTheme GetCurrentSelectedBoardTheme()
        {
            string currentThemeName = Settings.Settings.CurrentBoardTheme;
            Themes.BoardTheme theme = BoardThemes.FirstOrDefault(b => string.Equals(b.Name, currentThemeName, StringComparison.OrdinalIgnoreCase));
            return theme;
        }


        public Themes.PieceTheme GetCurrentSelectedPieceTheme()
        {
            string currentThemeName = Settings.Settings.CurrentPieceTheme;
            Themes.PieceTheme theme = PieceThemes.FirstOrDefault(p => string.Equals(p.Name, currentThemeName, StringComparison.OrdinalIgnoreCase));
            return theme;
        }


        private Themes.BoardTheme _selectedBoardTheme;
        public Themes.BoardTheme SelectedBoardTheme
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


        private Themes.PieceTheme _selectedPieceTheme;
        public Themes.PieceTheme SelectedPieceTheme
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
