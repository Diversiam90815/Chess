using Chess.UI.Images;
using Chess.UI.Wrappers;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.UI.Xaml.Media;
using System;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using static Chess.UI.Services.EngineAPI;


namespace Chess.UI.ViewModels
{
    public partial class MainMenuViewModel : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        // Specific events for different actions
        public event Action ButtonClicked;
        public event Action StartGameRequested;
        public event Action SettingsRequested;
        public event Action MultiplayerRequested;
        public event Action QuitRequested;

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

        public void OnButtonClicked()
        {
            ButtonClicked?.Invoke();
        }

        public void OnStartGameRequested()
        {
            StartGameRequested?.Invoke();
        }

        public void OnSettingsRequested()
        {
            SettingsRequested?.Invoke();
        }

        public void OnMultiplayerRequested()
        {
            MultiplayerRequested?.Invoke();
        }

        public void OnQuitRequested()
        {
            QuitRequested?.Invoke();
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
