using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace Chess_UI.Services
{
    public class ThemeManager : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;


        public ThemeManager()
        {
        }


        private BoardTheme currentBoardTheme;
        public BoardTheme CurrentBoardTheme
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


        private PieceTheme currentPieceTheme;
        public PieceTheme CurrentPieceTheme
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