using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace Chess.UI.Audio.Core
{
    public interface IAudioEngine
    {
        // Lifecycle
        Task InitializeAsync();
        void Dispose();

        // Module Management
        void RegisterModule(AudioModuleType moduleType, IAudioModule module);
        T GetModule<T>(AudioModuleType moduleType) where T : class, IAudioModule;
        IAudioModule GetModule(AudioModuleType moduleType);
        void UnregisterModule(AudioModuleType moduleType);

        // Global controls
        void SetMasterVolume(float volume);
        float GetMasterVolume();

        // Events
        event EventHandler<AudioEngineEventArgs> ModuleRegistered;
        event EventHandler<AudioEngineEventArgs> ModuleUnregistered;
        event EventHandler<AudioEngineEventArgs> AudioError;
    }


    public interface IAudioModule
    {
        public string ModuleName { get; }
        public AudioModuleType ModuleType { get; }

        Task InitializeAsync();
        void Dispose();

        void SetVolume(float volume);
        float GetVolume();

        event EventHandler<AudioModuleEventArgs> StatusChanged;
    }


    public class AudioEngineEventArgs : EventArgs
    {
        public string ModuleName { get; }
        public AudioModuleType ModuleType { get; }

        public AudioEngineEventArgs(string moduleName, AudioModuleType moduleType)
        {
            ModuleName = moduleName;
            ModuleType = moduleType;
        }
    }

    public class AudioModuleEventArgs : EventArgs
    {
        public string ModuleName { get; }
        public string Status { get; }

        public AudioModuleEventArgs(string moduleName, string status)
        {
            ModuleName = moduleName;
            Status = status;
        }
    }


    public enum AudioModuleType
    {
        SFX,
        Music,
    }
}
