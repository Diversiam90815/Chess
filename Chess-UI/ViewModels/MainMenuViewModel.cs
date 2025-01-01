using Chess_UI.Configuration;
using Microsoft.UI.Dispatching;
using Microsoft.UI.Xaml.Media;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;


namespace Chess_UI.ViewModels
{
    public class MainMenuViewModel : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        private readonly DispatcherQueue DispatcherQueue;

        public Controller Controller;


        public MainMenuViewModel(DispatcherQueue dispatcher)
        {
            Controller = new Controller();
            DispatcherQueue = dispatcher;
        }


        private ImageSource startGameButtonImage = Images.GetImage(Images.MainMenuButton.StartGame);
        public ImageSource StartGameButtonImage
        {
            get => startGameButtonImage;
            set
            {
                if (startGameButtonImage != value)
                {
                    startGameButtonImage = value;
                    OnPropertyChanged();
                }
            }
        }


        private ImageSource settingButtonImage = Images.GetImage(Images.MainMenuButton.Settings);
        public ImageSource SettingButtonImage
        {
            get => settingButtonImage;
            set
            {
                if (settingButtonImage != value)
                {
                    settingButtonImage = value;
                    OnPropertyChanged();
                }
            }
        }


        private ImageSource multiplayerButtonImage = Images.GetImage(Images.MainMenuButton.Multiplayer);
        public ImageSource MultiplayerButtonImage
        {
            get => multiplayerButtonImage;
            set
            {
                if (multiplayerButtonImage != value)
                {
                    multiplayerButtonImage = value;
                    OnPropertyChanged();
                }
            }
        }


        private ImageSource endGameButtonImage = Images.GetImage(Images.MainMenuButton.EndGame);
        public ImageSource EndGameButtonImage
        {
            get => endGameButtonImage;
            set
            {
                if (endGameButtonImage != value)
                {
                    endGameButtonImage = value;
                    OnPropertyChanged();
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
