using Chess.UI.Audio.Core;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Media.Core;
using Windows.Media.Playback;

namespace Chess.UI.Audio.Modules
{
    public enum AtmosphereScenario
    {
        None,
        Tavern,
        Fireplace,
    }

    public class AtmosphereChangedEventArgs : EventArgs
    {
        public AtmosphereScenario Scenario { get; }
        public float Volume { get; }
        public DateTime ChangeTime { get; }

        public AtmosphereChangedEventArgs(AtmosphereScenario scenario, float volume)
        {
            Scenario = scenario;
            Volume = volume;
            ChangeTime = DateTime.Now;
        }
    }


    public interface IAtmosphereModule : IAudioModule
    {
        Task SetAtmosphereAsync(AtmosphereScenario scenario, float volume = 0.5f);
        Task StopAtmosphereAsync();

        AtmosphereScenario CurrentScenario { get; }
        bool IsPlaying { get; }
        bool InEnabled { get; set; }

        event EventHandler<AtmosphereChangedEventArgs> AtmosphereChanged;
    }


    public class AtmosphereModule : IAtmosphereModule, IDisposable
    {
        private readonly ConcurrentDictionary<AtmosphereScenario, MediaSource> _atmosphereCache;
        private MediaPlayer _currentPlayer;

        private float _moduleVolume = 1.0f;
        private float _masterVolume = 1.0f;
        private bool _isInitialized = false;

        // IAudioModule Properties
        public string ModuleName => "Atmosphere";
        public AudioModuleType ModuleType => AudioModuleType.Atmosphere;

        // IAtmosphereModule Properties
        public AtmosphereScenario CurrentScenario { get; private set; } = AtmosphereScenario.None;
        public bool IsPlaying => _currentPlayer?.PlaybackSession?.PlaybackState == MediaPlaybackState.Playing;
        public bool IsEnabled { get; set; } = true;

        // Events
        public event EventHandler<AudioModuleEventArgs> StatusChanged;
        public event EventHandler<AtmosphereChangedEventArgs> AtmosphereChanged;


        public AtmosphereModule()
        {
            _atmosphereCache = new ConcurrentDictionary<AtmosphereScenario, MediaSource>();
        }

    }
}
