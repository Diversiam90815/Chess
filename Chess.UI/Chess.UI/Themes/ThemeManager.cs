using Chess_UI.Themes.Interfaces;
using System;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using Chess_UI.Images;
using Chess_UI.Settings;

namespace Chess_UI.Themes
{
    public class ThemeManager : IThemeManager
    {
        public event PropertyChangedEventHandler PropertyChanged;


        public ThemeManager()
        {
            string configPieceTheme = Settings.Settings.CurrentPieceTheme;
            string configBoardTheme = Settings.Settings.CurrentBoardTheme;

            // Try to parse the config themes to enum values
            if (!Enum.TryParse<ImageServices.PieceTheme>(configPieceTheme, out var parsedPieceTheme))
            {
                parsedPieceTheme = ImageServices.PieceTheme.Standard;
            }
            CurrentPieceTheme = parsedPieceTheme;

            if (!Enum.TryParse<ImageServices.BoardTheme>(configBoardTheme, out var parsedBoardTheme))
            {
                parsedBoardTheme = ImageServices.BoardTheme.Wood;
            }
            CurrentBoardTheme = parsedBoardTheme;
        }


        private ImageServices.BoardTheme currentBoardTheme;
        public ImageServices.BoardTheme CurrentBoardTheme
        {
            get => currentBoardTheme;
            set
            {
                if (currentBoardTheme != value)
                {
                    currentBoardTheme = value;
                    OnPropertyChanged();
                }
            }
        }


        private ImageServices.PieceTheme currentPieceTheme;
        public ImageServices.PieceTheme CurrentPieceTheme
        {
            get => currentPieceTheme;
            set
            {
                if (currentPieceTheme != value)
                {
                    currentPieceTheme = value;
                    OnPropertyChanged();
                }
            }
        }


        protected void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
