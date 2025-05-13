using Chess_UI.Services;
using Chess_UI.Themes.Interfaces;
using System;
using System.ComponentModel;
using System.Runtime.CompilerServices;

namespace Chess_UI.Themes
{
    public class ThemeManager : IThemeManager
    {
        public event PropertyChangedEventHandler PropertyChanged;


        public ThemeManager()
        {
            string configPieceTheme = Configuration.CurrentPieceTheme;
            string configBoardTheme = Configuration.CurrentBoardTheme;

            // Try to parse the config themes to enum values
            if (!Enum.TryParse<Images.PieceTheme>(configPieceTheme, out var parsedPieceTheme))
            {
                parsedPieceTheme = Images.PieceTheme.Standard;
            }
            CurrentPieceTheme = parsedPieceTheme;

            if (!Enum.TryParse<Images.BoardTheme>(configBoardTheme, out var parsedBoardTheme))
            {
                parsedBoardTheme = Images.BoardTheme.Wood;
            }
            CurrentBoardTheme = parsedBoardTheme;
        }


        private Images.BoardTheme currentBoardTheme;
        public Images.BoardTheme CurrentBoardTheme
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


        private Images.PieceTheme currentPieceTheme;
        public Images.PieceTheme CurrentPieceTheme
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
