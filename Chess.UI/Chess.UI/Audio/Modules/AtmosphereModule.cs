using Chess.UI.Audio.Core;
using Chess.UI.Services;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.IO;
using System.Threading.Tasks;
using Windows.Media.Core;
using Windows.Media.Playback;
using Windows.Storage;


namespace Chess.UI.Audio.Modules
{
    public enum AtmosphereScenario
    {
        Tavern,
        Fireplace,
        Forest,
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
        Task SetAtmosphereAsync(AtmosphereScenario scenario);
        void StopAtmosphereAsync();
        void SetCrossfadeDuration(float seconds);

        AtmosphereScenario CurrentScenario { get; }
        bool IsPlaying { get; }
        bool IsEnabled { get; set; }

        event EventHandler<AtmosphereChangedEventArgs> AtmosphereChanged;
    }


    public class AtmosphereModule : IAtmosphereModule, IDisposable
    {
        private readonly ConcurrentDictionary<AtmosphereScenario, MediaSource> _atmosphereCache;
        private MediaPlayer _currentPlayer;
        private MediaPlayer _crossfadePlayer;
        private readonly object _playerLock = new();

        private float _moduleVolume = 1.0f;
        private float _masterVolume = 1.0f;
        private float _crossfadeDuration = 2.0f; // seconds
        private bool _isInitialized = false;

        // IAudioModule Properties
        public string ModuleName => "Atmosphere";
        public AudioModuleType ModuleType => AudioModuleType.Atmosphere;

        // IAtmosphereModule Properties
        public AtmosphereScenario CurrentScenario { get; private set; } = AtmosphereScenario.Forest;
        public bool IsPlaying => _currentPlayer?.PlaybackSession?.PlaybackState == MediaPlaybackState.Playing;
        public bool IsEnabled { get; set; } = true;

        // Events
        public event EventHandler<AudioModuleEventArgs> StatusChanged;
        public event EventHandler<AtmosphereChangedEventArgs> AtmosphereChanged;


        public AtmosphereModule()
        {
            _atmosphereCache = new ConcurrentDictionary<AtmosphereScenario, MediaSource>();
        }


        public async Task InitializeAsync()
        {
            if (_isInitialized) return;

            try
            {
                await PreloadAtmosphereTracksAsync();
                InitializeMediaPlayers();

                _isInitialized = true;
                StatusChanged?.Invoke(this, new AudioModuleEventArgs(ModuleName, "Initialized"));
            }
            catch (Exception ex)
            {
                StatusChanged?.Invoke(this, new AudioModuleEventArgs(ModuleName, $"Failed to initialize: {ex.Message}"));
                throw;
            }
        }


        // Volume Management - IAudioModule Implementation
        public void SetModuleVolume(float volume)
        {
            _moduleVolume = Math.Clamp(volume, 0.0f, 1.0f);
            UpdateCurrentPlayerVolume();
        }


        public float GetModuleVolume() => _moduleVolume;


        public void SetMasterVolume(float masterVolume)
        {
            _masterVolume = Math.Clamp(masterVolume, 0.0f, 1.0f);
            UpdateCurrentPlayerVolume();
        }


        public float GetEffectiveVolume() => _moduleVolume * _masterVolume;


        public void SetCrossfadeDuration(float seconds)
        {
            _crossfadeDuration = Math.Max(0.1f, seconds);
        }


        public async Task SetAtmosphereAsync(AtmosphereScenario scenario)
        {
            if (!_isInitialized || !IsEnabled) return;

            try
            {
                var mediaSource = await GetMediaSourceAsync(scenario);
                if (mediaSource == null) return;

                float volume = GetEffectiveVolume();

                lock (_playerLock)
                {
                    if (_currentPlayer?.PlaybackSession?.PlaybackState == MediaPlaybackState.Playing)
                    {
                        //Start crossfade
                        StartCrossfade(mediaSource, volume);
                    }
                    else
                    {
                        // Start direct play
                        StartDirectPlayback(mediaSource, volume);
                    }
                }

                CurrentScenario = scenario;
                AtmosphereChanged?.Invoke(this, new AtmosphereChangedEventArgs(scenario, volume * GetEffectiveVolume()));
            }
            catch (Exception ex)
            {
                Logger.LogError($"Failed to set atmosphere {scenario}: {ex.Message}");
                StatusChanged?.Invoke(this, new AudioModuleEventArgs(ModuleName, $"Failed to set atmosphere {scenario}: {ex.Message}"));
            }
        }


        public void StopAtmosphereAsync()
        {
            if (!_isInitialized) return;

            lock (_playerLock)
            {
                _currentPlayer?.Pause();
                _crossfadePlayer?.Pause();
            }

            AtmosphereChanged?.Invoke(this, new AtmosphereChangedEventArgs(CurrentScenario, 0.0f));
        }


        private async Task PreloadAtmosphereTracksAsync()
        {
            var loadTasks = new List<Task>();

            foreach (AtmosphereScenario scenario in Enum.GetValues<AtmosphereScenario>())
            {
                loadTasks.Add(LoadAtmosphereTrackAsync(scenario));
            }

            await Task.WhenAll(loadTasks);
        }


        private async Task LoadAtmosphereTrackAsync(AtmosphereScenario scenario)
        {
            try
            {
                var filePath = GetAtmosphereTrackPath(scenario);
                if (!File.Exists(filePath))
                {
                    Logger.LogWarning($"Atmosphere track not found: {filePath}");
                    return;
                }

                var file = await StorageFile.GetFileFromPathAsync(filePath);
                var mediaSource = MediaSource.CreateFromStorageFile(file);

                _atmosphereCache.TryAdd(scenario, mediaSource);
            }
            catch (Exception ex)
            {
                Logger.LogWarning($"Failed to load atmosphere track {scenario}: {ex.Message}");
            }
        }


        private async Task<MediaSource> GetMediaSourceAsync(AtmosphereScenario scenario)
        {
            if (_atmosphereCache.TryGetValue(scenario, out var mediaSource))
            {
                return mediaSource;
            }

            // Try to load on-demand if not in cache
            await LoadAtmosphereTrackAsync(scenario);
            _atmosphereCache.TryGetValue(scenario, out mediaSource);
            return mediaSource;
        }


        private void InitializeMediaPlayers()
        {
            _currentPlayer = new MediaPlayer
            {
                AudioCategory = MediaPlayerAudioCategory.GameMedia,
                IsLoopingEnabled = true
            };
            _currentPlayer.MediaEnded += OnMediaPlayerEnded;
            _currentPlayer.MediaFailed += OnMediaPlayerFailed;

            _crossfadePlayer = new MediaPlayer
            {
                AudioCategory = MediaPlayerAudioCategory.GameMedia,
                IsLoopingEnabled = true
            };
            _crossfadePlayer.MediaEnded += OnCrossfadePlayerEnded;
            _crossfadePlayer.MediaFailed += OnMediaPlayerFailed;
        }


        private void StartDirectPlayback(MediaSource mediaSource, float volume)
        {
            _currentPlayer.Source = mediaSource;
            _currentPlayer.Volume = Math.Clamp(volume * GetEffectiveVolume(), 0.0f, 1.0f);
            _currentPlayer.Play();
        }


        private void StartCrossfade(MediaSource mediaSource, float targetVolume)
        {
            // Setup crossfade player
            if (_crossfadePlayer is null)
            {
                _crossfadePlayer = new MediaPlayer
                {
                    AudioCategory = MediaPlayerAudioCategory.GameMedia,
                    IsLoopingEnabled = true
                };
                _crossfadePlayer.MediaEnded += OnCrossfadePlayerEnded;
                _crossfadePlayer.MediaFailed += OnMediaPlayerFailed;
            }

            _crossfadePlayer.Source = mediaSource;
            _crossfadePlayer.Volume = 0.0f;
            _crossfadePlayer.Play();

            // Start crossfade animation
            Task.Run(async () => await PerformCrossfade(targetVolume));
        }


        private void UpdateCurrentPlayerVolume()
        {
            lock (_playerLock)
            {
                if (_currentPlayer != null)
                {
                    var currentVolume = _currentPlayer.Volume / Math.Max(0.001f, GetEffectiveVolume());
                    _currentPlayer.Volume = Math.Clamp(currentVolume * GetEffectiveVolume(), 0.0f, 1.0f);
                }
            }
        }


        private async Task PerformCrossfade(float targetVolume)
        {
            int steps = (int)(_crossfadeDuration * 10);
            int stepDelay = (int)((_crossfadeDuration * 1000) / steps);
            float effectiveVolume = targetVolume * GetEffectiveVolume();

            for (int i = 0; i <= steps; i++)
            {
                float progress = (float)i / steps;

                lock (_playerLock)
                {
                    if (_currentPlayer != null)
                        _currentPlayer.Volume = Math.Clamp(effectiveVolume * (1.0 - progress), 0.0f, 1.0f);

                    if (_crossfadePlayer != null)
                        _crossfadePlayer.Volume = Math.Clamp(effectiveVolume * progress, 0.0f, 1.0f);
                }

                await Task.Delay(stepDelay);
            }

            // Swap the players
            lock (_playerLock)
            {
                var tmp = _currentPlayer;
                _currentPlayer = _crossfadePlayer;
                _crossfadePlayer = tmp;

                if (_crossfadePlayer != null)
                {
                    _crossfadePlayer.Pause();
                    _crossfadePlayer.Source = null;
                }
                _crossfadePlayer = null;
            }
        }


        private void OnMediaPlayerEnded(MediaPlayer sender, object args)
        {
            // Should not happen with looping enabled, but just in case
            Logger.LogWarning("Atmosphere track ended unexpectedly");
        }


        private void OnCrossfadePlayerEnded(MediaPlayer sender, object args)
        {
            // Crossfade player ended - should not happen during normal operation
        }


        private void OnMediaPlayerFailed(MediaPlayer sender, MediaPlayerFailedEventArgs args)
        {
            Logger.LogError($"Atmosphere player failed: {args.ErrorMessage}");
            StatusChanged?.Invoke(this, new AudioModuleEventArgs(ModuleName, $"Playback failed: {args.ErrorMessage}"));
        }


        private string GetAtmosphereTrackPath(AtmosphereScenario scenario)
        {
            var baseDir = AppDomain.CurrentDomain.BaseDirectory;
            return Path.Combine(baseDir, "Assets", "Audio", "Atmosphere", $"{scenario}.wav");
        }


        public void Dispose()
        {
            _isInitialized = false;

            lock (_playerLock)
            {
                _currentPlayer?.Dispose();
                _crossfadePlayer?.Dispose();
            }

            // Dispose all cached media sources
            foreach (var mediaSource in _atmosphereCache.Values)
            {
                mediaSource?.Dispose();
            }
            _atmosphereCache.Clear();
        }
    }
}
