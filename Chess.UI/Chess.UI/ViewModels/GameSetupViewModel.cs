using Chess.UI.Services;
using Chess.UI.Wrappers;
using Microsoft.Extensions.DependencyInjection;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices.Marshalling;
using static Chess.UI.Services.EngineAPI;


namespace Chess.UI.ViewModels
{
    class GameSetupViewModel : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        private GameConfigurationBuilder _configuration;

        private readonly IDispatcherQueueWrapper _dispatcherQueue;

        private readonly IGameConfigurationService _configurationService;


        public GameSetupViewModel(IDispatcherQueueWrapper dispatcher)
        {
            _dispatcherQueue = dispatcher;

            _configuration = App.Current.Services.GetService<GameConfigurationBuilder>();
            _configurationService = App.Current.Services.GetService<IGameConfigurationService>();
        }


        public void LocalCoopInitiated()
        {
            GameMode = EngineAPI.GameModeSelection.LocalCoop;

            StartGame();
        }


        public void CPUGameInitiated()
        {
            GameMode = EngineAPI.GameModeSelection.VsCPU;

            PlayerConfigVisible = false;
            CPUConfigVisible = true;
        }


        private void StartGame()
        {
            // Set the values 
            _configuration.SetGameMode(GameMode);

            if (_gameMode == GameModeSelection.VsCPU)
            {
                _configuration.SetPlayerColor(PlayerColor);
                _configuration.SetCPUDifficulty(_cpudifficulty);
            }

            var config = _configuration.GetConfiguration();

            _configurationService.StartGameAsync(config);
        }


        private GameModeSelection _gameMode;
        public GameModeSelection GameMode
        {
            get => _gameMode;
            set
            {
                if (_gameMode != value)
                {
                    _gameMode = value;
                    OnPropertyChanged();
                }
            }
        }


        // Game Config Menu : CPU Configuration
        private bool _cpuConfigVisible = false;
        public bool CPUConfigVisible
        {
            get => _cpuConfigVisible;
            set
            {
                if (value != _cpuConfigVisible)
                {
                    _cpuConfigVisible = value;
                    OnPropertyChanged();
                }
            }
        }

        // Game Config Menu : CPU or Coop Game
        private bool _playerConfigVisible = true;
        public bool PlayerConfigVisible
        {
            get => _playerConfigVisible;
            set
            {
                if (value != _playerConfigVisible)
                {
                    _playerConfigVisible = value;
                    OnPropertyChanged();
                }
            }
        }


        private EngineAPI.PlayerColor _playerColor;
        public EngineAPI.PlayerColor PlayerColor
        {
            get => _playerColor;
            set
            {
                if (value != _playerColor)
                {
                    _playerColor = value;
                    OnPropertyChanged();
                }
            }
        }


        private EngineAPI.CPUDifficulty _cpudifficulty;
        public EngineAPI.CPUDifficulty CPUDifficulty
        {
            get => _cpudifficulty;
            set
            {
                if (value != _cpudifficulty)
                {
                    _cpudifficulty = value;
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
