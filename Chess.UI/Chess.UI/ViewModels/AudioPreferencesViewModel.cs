using Chess.UI.Audio.Modules;
using Chess.UI.Audio.Services;
using Chess.UI.Styles;
using Chess.UI.Wrappers;
using Microsoft.Extensions.DependencyInjection;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using WinRT.ChessVtableClasses;


namespace Chess.UI.ViewModels
{
    public record AtmosphereScenarioItem
    {
        public string DisplayName { get; }
        public Audio.Modules.AtmosphereScenario Scenario { get; }

        public AtmosphereScenarioItem(Audio.Modules.AtmosphereScenario scenario)
        {
            Scenario = scenario;
            DisplayName = scenario.ToString();
        }
    }


    public class AudioPreferencesViewModel
    {
        public ObservableCollection<AtmosphereScenarioItem> AtmosphereSoundscapes { get; }

        private readonly IChessAudioService _audioService;


        public AudioPreferencesViewModel()
        {
            _audioService = App.Current.Services.GetService<IChessAudioService>();

            AtmosphereSoundscapes = new ObservableCollection<AtmosphereScenarioItem>();

            IntiializeAtmosphereScenarios();
            InitializeValues();
        }


        private void InitializeValues()
        {
            float masterEngineVolume = _audioService.GetMasterVolume();
            int masterVolume = (int)masterEngineVolume * 100;

            float sfxEngineVolume = _audioService.GetSFXVolume();
            int sfxVolume = (int)sfxEngineVolume * 100;

            float atmosEngineVolume = _audioService.GetAtmosphereVolume();
            int atmosVolume = (int)atmosEngineVolume * 100;

            bool sfxEnabled = _audioService.GetSFXEnabled();
            bool atmosEnabled = _audioService.GetAtmosphereEnabled();

            AtmosphereScenario currentScenario = _audioService.GetCurrentAtmosphere();

            SfxEnabled = sfxEnabled;
            AtmosEnabled = atmosEnabled;

            MasterVolume = masterVolume;
            SfxVolume = sfxVolume;
            AtmosVolume = atmosVolume;

            SelectedAtmosphereSoundscape = AtmosphereSoundscapes.FirstOrDefault(a => a.Scenario == currentScenario);
        }


        private void IntiializeAtmosphereScenarios()
        {
            foreach (var scenario in Enum.GetValues<AtmosphereScenario>())
            {
                AtmosphereSoundscapes.Add(new AtmosphereScenarioItem(scenario));
            }
        }


        private AtmosphereScenarioItem selectedAtmosphereSoundscape;
        public AtmosphereScenarioItem SelectedAtmosphereSoundscape
        {
            get => selectedAtmosphereSoundscape;
            set
            {
                if (value == selectedAtmosphereSoundscape) return;
                selectedAtmosphereSoundscape = value;
                _audioService.SetAtmosphereAsync(value.Scenario);
            }
        }


        private int masterVolume;
        public int MasterVolume
        {
            get => masterVolume;
            set
            {
                if (value == masterVolume) return;
                masterVolume = value;
                SetMasterVolumeInEngine(masterVolume);
            }
        }


        private int sfxVolume;
        public int SfxVolume
        {
            get => sfxVolume;
            set
            {
                if (value == sfxVolume) return;
                sfxVolume = value;
                SetSfxVolumeInEngine(sfxVolume);
            }
        }


        private int atmosVolume;
        public int AtmosVolume
        {
            get => atmosVolume;
            set
            {
                if (value == atmosVolume) return;
                atmosVolume = value;
                SetAtmosVolumeInEngine(atmosVolume);
            }
        }


        private bool sfxEnabled;
        public bool SfxEnabled
        {
            get => sfxEnabled;
            set
            {
                if (value == sfxEnabled) return;
                sfxEnabled = value;
                _audioService.SetSFXEnabled(value);
            }
        }


        private bool atmosEnabled;
        public bool AtmosEnabled
        {
            get => atmosEnabled;
            set
            {
                if (value == atmosEnabled) return;
                atmosEnabled = value;
                _audioService.SetAtmosphereEnabled(value);
            }
        }


        public void SetSfxVolumeInEngine(int volume)
        {
            float engineVolume = volume / 100;
            _audioService.SetSFXVolume(engineVolume);
        }


        public void SetAtmosVolumeInEngine(int volume)
        {
            float engineVolume = volume / 100;
            _audioService.SetAtmosphereVolume(engineVolume);
        }


        public void SetMasterVolumeInEngine(int volume)
        {
            float engineVolume = volume / 100;
            _audioService.SetMasterVolume(engineVolume);
        }







    }
}
