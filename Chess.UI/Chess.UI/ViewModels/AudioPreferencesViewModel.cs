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

    internal class AudioPreferencesViewModel
    {
        public ObservableCollection<string> AtmosphereSoundscapes { get; }

        private readonly IDispatcherQueueWrapper _dispatcherQueue;

        private readonly IChessAudioService _audioService;

        public AudioPreferencesViewModel(IDispatcherQueueWrapper dispatcher)
        {
            _audioService = App.Current.Services.GetService<IChessAudioService>();
            _dispatcherQueue = dispatcher;

            AtmosphereSoundscapes = new ObservableCollection<string>();
            AtmosphereSoundscapes.Add("Test1");
            AtmosphereSoundscapes.Add("Test2");
            AtmosphereSoundscapes.Add("Test3");
        }


        private string selectedAtmosphereSoundscape;
        public string SelectedAtmosphereSoundscape
        {
            get => selectedAtmosphereSoundscape;
            set
            {
                if (value == selectedAtmosphereSoundscape) return;
                selectedAtmosphereSoundscape = value;
                _audioService.SetAtmosphereAsync(Audio.Modules.AtmosphereScenario.None);    // TODO
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
                _audioService.SetSFXVolume(value / 100);
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
                _audioService.SetAtmosphereVolume(value / 100);
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







    }
}
