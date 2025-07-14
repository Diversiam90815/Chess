using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Chess.UI.Audio.Core
{
    public class AudioEngine : IAudioEngine, IDisposable
    {
        private readonly ConcurrentDictionary<AudioModuleType, IAudioModule> _modules;
        private float _masterVolume = 1.0f;
        private bool _isInitialized = false;

        public event EventHandler<AudioEngineEventArgs> ModuleRegistered;
        public event EventHandler<AudioEngineEventArgs> ModuleUnregistered;
        public event EventHandler<AudioErrorEventArgs> AudioError;


        public AudioEngine()
        {
            _modules = new ConcurrentDictionary<AudioModuleType, IAudioModule>();
        }

        public void RegisterModule(AudioModuleType type, IAudioModule moduley)
        {

        }

        public T GetModule<T>(AudioModuleType moduleType) where T : class, IAudioModule
        {

        }

        public IAudioModule GetModule(AudioModuleType moduleType)
        {

        }

        public void UnregisterModule(AudioModuleType moduleType)
        {
        }

        public void SetMasterVolume(float masterVolume)
        {
        }

        public float GetMasterVolume()
        {

        }

        public void Dispose()
        {
        }
    }
