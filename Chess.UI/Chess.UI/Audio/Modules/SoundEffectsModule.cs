using Chess.UI.Audio.Core;
using Chess.UI.Services;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Media.Core;
using Windows.Media.Playback;
using Windows.Storage;

namespace Chess.UI.Audio.Modules
{
    public enum SoundEffect
    {
        // Piece Movement
        PieceMove,
        PieceCapture,
        Castling,
        EnPassant,
        PawnPromotion,

        // States
        Check,
        Checkmate,
        Stalemate,
        Draw,

        // Flow
        GameStart,
        GameEnd,
        TurnChange,

        // UI
        ButtonClick,
        MenuOpen,
        ItemSelected,

        // Multiplayer
        OpponentJoined,
        OpponentLeft,
        ChatMessageReceived,
    }


    public class SoundPlayedEventArgs : EventArgs
    {
        public SoundEffect Effect { get; }
        public float Volume { get; }
        public DateTime PlayTime { get; }

        public SoundPlayedEventArgs(SoundEffect effect, float volume)
        {
            Effect = effect;
            Volume = volume;
            PlayTime = DateTime.Now;
        }
    }

    public interface ISoundEffectsModule : IAudioModule
    {
        Task PlaySoundAsync(SoundEffect effect, float volume = 1.0f);

        Task PlaySoundAsync(SoundEffect effect, float volume, float pitch);

        void SetMaxConcurrentSounds(int maxSounds);

        bool IsEnabled { get; set; }

        event EventHandler<SoundPlayedEventArgs> SoundPlayed;
    }


    public class SoundEffectsModule : ISoundEffectsModule, IDisposable
    {
        private readonly ConcurrentDictionary<SoundEffect, MediaSource> _soundCache;
        private readonly ConcurrentQueue<MediaPlayer> _playerPool;
        private readonly object _mediaPlayerPoolLock = new();

        private float _moduleVolume = 1.0f;      // Module's own volume setting
        private float _masterVolume = 1.0f;      // Master volume from AudioEngine
        private int _maxConcurrentSounds = 8;
        private bool _isInitialized = false;


        // IAudioModule Properties
        public string ModuleName => "SFX";
        public AudioModuleType ModuleType => AudioModuleType.SFX;


        // ISoundEffectsModule Properties
        public bool IsEnabled { get; set; } = true;


        // Events
        public event EventHandler<AudioModuleEventArgs> StatusChanged;
        public event EventHandler<SoundPlayedEventArgs> SoundPlayed;


        public SoundEffectsModule()
        {
            _soundCache = new ConcurrentDictionary<SoundEffect, MediaSource>();
            _playerPool = new ConcurrentQueue<MediaPlayer>();
        }


        public async Task InitializeAsync()
        {
            if (_isInitialized) return;

            try
            {
                await PreloadSoundEffectsAsync();
                InitializeMediaPlayerPool();

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
        }


        public float GetModuleVolume() => _moduleVolume;

        public void SetMasterVolume(float masterVolume)
        {
            _masterVolume = Math.Clamp(masterVolume, 0.0f, 1.0f);
        }

        public float GetEffectiveVolume() => _moduleVolume * _masterVolume;


        public void SetMaxConcurrentSounds(int maxSounds)
        {
            _maxConcurrentSounds = Math.Max(1, maxSounds);
        }


        private async Task PreloadSoundEffectsAsync()
        {
            var loadTasks = new List<Task>();

            foreach (SoundEffect effect in Enum.GetValues<SoundEffect>())
            {
                loadTasks.Add(LoadSoundEffectAsync(effect));
            }

            await Task.WhenAll(loadTasks);
        }


        private async Task LoadSoundEffectAsync(SoundEffect effect)
        {
            try
            {
                var filePath = GetSoundEffectPath(effect);
                if (!File.Exists(filePath))
                {
                    Logger.LogWarning($"Sound file not found : {filePath}");
                    return;
                }

                var file = await StorageFile.GetFileFromPathAsync(filePath);
                var mediaSource = MediaSource.CreateFromStorageFile(file);

                _soundCache.TryAdd(effect, mediaSource);
            }
            catch (Exception ex)
            {
                Logger.LogWarning($"Failed to load sound effect {effect} : {ex.Message}");
            }
        }


        private async Task<MediaSource> GetMediaSourceAsync(SoundEffect effect)
        {
            if (_soundCache.TryGetValue(effect, out var mediaSource))
            {
                return mediaSource;
            }

            // Try to load on-demand if not in cache
            await LoadSoundEffectAsync(effect);
            _soundCache.TryGetValue(effect, out mediaSource);
            return mediaSource;
        }


        private void InitializeMediaPlayerPool()
        {
            for (int i = 0; i < _maxConcurrentSounds; ++i)
            {
                var mediaPlayer = new MediaPlayer();
                _playerPool.Enqueue(mediaPlayer);
            }
        }


        private MediaPlayer GetMediaPlayerFromPool()
        {
            lock (_mediaPlayerPoolLock)
            {
                if (_playerPool.TryDequeue(out var player))
                {
                    return player;
                }
            }

            // If pool is empty, create new temporary player
            return new MediaPlayer();
        }


        private void ReturnPlayerToPool(MediaPlayer player)
        {
            if (player == null) return;

            try
            {
                player.Source = null;
                player.PlaybackRate = 1.0;

                lock (_mediaPlayerPoolLock)
                {
                    if (_playerPool.Count < _maxConcurrentSounds)
                    {
                        _playerPool.Enqueue(player);
                        return;
                    }
                }

                // Pool is full, dispose the player
                player.Dispose();
            }
            catch (Exception ex)
            {
                Logger.LogWarning($"Error returning player to pool: {ex.Message}");
                player?.Dispose();
            }
        }


        public async Task PlaySoundAsync(SoundEffect effect, float volume = 1.0f)
        {
            await PlaySoundAsync(effect, volume, 1.0f);
        }


        public async Task PlaySoundAsync(SoundEffect effect, float volume, float pitch)
        {
            if (!_isInitialized || !IsEnabled) return;

            try
            {
                var mediaSource = await GetMediaSourceAsync(effect);
                if (mediaSource == null) return;

                var mediaPlayer = GetMediaPlayerFromPool();
                if (mediaPlayer == null) return;

                // Calculate final volume
                var effectiveVolume = volume * GetEffectiveVolume();

                mediaPlayer.Source = mediaSource;
                mediaPlayer.Volume = Math.Clamp(effectiveVolume, 0.0f, 1.0f);
                mediaPlayer.PlaybackRate = Math.Clamp(pitch, 0.25, 4.0);

                // Return player to pool when playback ends
                mediaPlayer.MediaEnded += (s, e) => ReturnPlayerToPool(mediaPlayer);
                mediaPlayer.MediaFailed += (s, e) => ReturnPlayerToPool(mediaPlayer);

                mediaPlayer.Play();

                SoundPlayed?.Invoke(this, new SoundPlayedEventArgs(effect, effectiveVolume));
            }
            catch (Exception ex)
            {
                StatusChanged?.Invoke(this, new AudioModuleEventArgs(ModuleName, $"Play failed for {effect}: {ex.Message}"));
            }
        }


        private string GetSoundEffectPath(SoundEffect effect)
        {
            var baseDir = AppDomain.CurrentDomain.BaseDirectory;
            return Path.Combine(baseDir, "Assets", "Audio", "SoundEffects", $"{effect}.wav");
        }


        public void Dispose()
        {
            _isInitialized = false;

            // Dispose all cached media sources
            foreach (var mediaSource in _soundCache.Values)
            {
                mediaSource?.Dispose();
            }
            _soundCache.Clear();

            // Dispose all players in pool
            while (_playerPool.TryDequeue(out var player))
            {
                player?.Dispose();
            }
        }

    }
}
