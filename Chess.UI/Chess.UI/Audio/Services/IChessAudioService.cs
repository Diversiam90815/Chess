using Chess.UI.Audio.Modules;
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
        void StopAtmosphereAsync();

        // Settings
        void SetSFXEnabled(bool enabled);
        void SetSFXVolume(float volume);
        void SetAtmosphereEnabled(bool enabled);
        void SetAtmosphereVolume(float volume);
        void SetMasterVolume(float volume);

        ISoundEffectsModule SoundEffectsModule { get; }
        IAtmosphereModule AtmosphereModule { get; }
    }
}
