﻿using Chess.UI.Audio.Modules;
using System.Threading.Tasks;
using static Chess.UI.Services.EngineAPI;

namespace Chess.UI.Audio.Services
{
    public enum UIInteraction
    {
        ButtonClick,
        MenuOpen,
        ItemSelected,
        PieceMove,
        PieceSelect
    }


    public interface IChessAudioService
    {
        Task InitializeAsync();

        // Chess Game Events
        Task HandleMoveAsync(PossibleMoveInstance move);
        Task HandleEndGameStateAsync(EndGameState state);
        Task HandleGameStartAsync();
        Task HandleGameEndAsync();

        // UI Interaction
        Task HandleUIInteractionAsync(UIInteraction interaction);

        // Atmosphere Control
        Task SetAtmosphereAsync(AtmosphereScenario scenario, float volume = 0.5f);
        AtmosphereScenario GetCurrentAtmosphere();
        void StopAtmosphereAsync();

        // Settings
        bool GetSFXEnabled();
        void SetSFXEnabled(bool enabled);
        float GetSFXVolume();
        void SetSFXVolume(float volume);
        bool GetAtmosphereEnabled();
        void SetAtmosphereEnabled(bool enabled);
        float GetAtmosphereVolume();
        void SetAtmosphereVolume(float volume);
        float GetMasterVolume();
        void SetMasterVolume(float volume);

        ISoundEffectsModule SoundEffectsModule { get; }
        IAtmosphereModule AtmosphereModule { get; }
    }
}
