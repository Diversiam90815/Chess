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
        void RegisterModule<T>(T module) where T : IAudioModule;
        T GetModule<T>() where T : IAudioModule;
        void UnregisterModule<T>() where T : IAudioModule;

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

    }

    public class AudioEngineEventArgs : EventArgs
    { }
}
