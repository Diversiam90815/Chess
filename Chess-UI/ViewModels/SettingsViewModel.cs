using Microsoft.UI.Dispatching;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using Chess_UI.Services;


namespace Chess_UI.ViewModels
{
    public class SettingsViewModel : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        private readonly DispatcherQueue DispatcherQueue;

        private Configuration Configuration;

        private ThemeLoader ThemeLoader;

        private List<BoardTheme> BoardThemes;

        private List<PieceTheme> PieceThemes;


        public SettingsViewModel(DispatcherQueue dispatcherQueue)
        {
            this.DispatcherQueue = dispatcherQueue;
            ThemeLoader = new();
            PieceThemes = ThemeLoader.LoadPieceThemes();
            BoardThemes = ThemeLoader.LoardBoardThemes();
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
