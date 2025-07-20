using Chess.UI.Audio.Core;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

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
    }
}
