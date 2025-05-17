using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace Chess.UI.Services
{
    public class ThemeManager : INotifyPropertyChanged
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
