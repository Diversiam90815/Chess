using Chess_UI.Images;
using Chess_UI.Services;
using Chess_UI.Wrappers;
using Microsoft.Extensions.DependencyInjection;
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

        private readonly IDispatcherQueueWrapper _dispatcherQueue;

        private readonly IImageService _imageServices;

        public MainMenuViewModel(IDispatcherQueueWrapper dispatcher)
        {
            _dispatcherQueue = dispatcher;
            _imageServices = App.Current.Services.GetService<IImageService>();

            Init();
        }


        private void Init()
        {
            StartGameButtonImage = _imageServices.GetImage(ImageServices.MainMenuButton.StartGame);
            SettingButtonImage = _imageServices.GetImage(ImageServices.MainMenuButton.Settings);
            MultiplayerButtonImage = _imageServices.GetImage(ImageServices.MainMenuButton.Multiplayer);
            EndGameButtonImage = _imageServices.GetImage(ImageServices.MainMenuButton.EndGame);
        }

        private ImageSource startGameButtonImage;
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


        private ImageSource settingButtonImage;
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


        private ImageSource multiplayerButtonImage;
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


        private ImageSource endGameButtonImage;
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
            _dispatcherQueue.TryEnqueue(() =>
            {
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
            });
        }

    }
}
