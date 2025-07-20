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
        Task HandleMoveAsync(PossibleMoveInstance move, bool isCapture);
        Task HandleEndGameStateAsync(EndGameState state);
        Task HandleCheckAsync();
        Task HandleGameStartAsync();
        Task HandleGameEndAsync();

        // UI Interaction
        Task HandleUIInteractionAsync(UIInteraction interaction);

        // Settings
        void SetSFXEnabled(bool enabled);
        void SetSFXVolume(float volume);
        void SetMasterVolume(float volume);

        ISoundEffectsModule SoundEffectsModule { get; }
    }
}
