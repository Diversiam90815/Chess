using Chess.UI.Services;
using Chess.UI.Wrappers;
using Microsoft.Extensions.DependencyInjection;
using System.ComponentModel;
using System.Runtime.CompilerServices;


namespace Chess.UI.ViewModels
{
    class GameSetupViewModel : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        private GameConfigurationBuilder _configuration;

        private readonly IDispatcherQueueWrapper _dispatcherQueue;


        public GameSetupViewModel(IDispatcherQueueWrapper dispatcher)
        {
            _dispatcherQueue = dispatcher;
            _configuration = App.Current.Services.GetService<GameConfigurationBuilder>();
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


        private EngineAPI.PlayerColor _cpuColor;
        public EngineAPI.PlayerColor CpuColor
        {
            get => _cpuColor;
            set
            {
                if (value != _cpuColor)
                {
                    _cpuColor = value;
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
