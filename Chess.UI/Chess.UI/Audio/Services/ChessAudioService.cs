using Chess.UI.Audio.Core;
using Chess.UI.Audio.Modules;
using Chess.UI.Models.Interfaces;
using Chess.UI.ViewModels;
using Microsoft.Extensions.DependencyInjection;
using System.Threading.Tasks;
using static Chess.UI.Services.EngineAPI;


namespace Chess.UI.Audio.Services
{
    public class ChessAudioService : IChessAudioService
    {
        private readonly IAudioEngine _audioEngine;
        private ISoundEffectsModule _soundEffectsModule;

        public ISoundEffectsModule SoundEffectsModule => _soundEffectsModule;


        public ChessAudioService(IAudioEngine engine)
        {
            _audioEngine = engine;
        }


        public async Task InitializeAsync()
        {
            _soundEffectsModule = new SoundEffectsModule();
            _audioEngine.RegisterModule(AudioModuleType.SFX, _soundEffectsModule);

            await _audioEngine.InitializeAsync();

            // Subscribe to the events from the viewmodels
            SubscribeToViewModelEvents();
        }


        private void SubscribeToViewModelEvents()
        {
            var mainMenuViewModel = App.Current.Services.GetService<MainMenuViewModel>();
            mainMenuViewModel.ButtonClicked += () => _ = Task.Run(async () => await HandleUIInteractionAsync(UIInteraction.ButtonClick));

            var chessboardVM = App.Current.Services.GetService<ChessBoardViewModel>();
            chessboardVM.ButtonClicked += () => _ = Task.Run(async () => await HandleUIInteractionAsync(UIInteraction.ButtonClick));

            var moveModel = App.Current.Services.GetService<IMoveModel>();
            moveModel.GameOverEvent += (EndGameState state, PlayerColor player) => _ = Task.Run(async () => await HandleEndGameStateAsync(state));
            moveModel.ChesspieceSelected += () => _ = Task.Run(async () => await HandleUIInteractionAsync(UIInteraction.PieceSelect));

            var backendCom = App.Current.ChessLogicCommunication;
            backendCom.MoveExecuted += (PossibleMoveInstance move) => _ = Task.Run(async () => await HandleMoveAsync(move));
        }


        public async Task HandleMoveAsync(PossibleMoveInstance move)
        {
            bool isCapture = move.type.HasFlag(MoveTypeInstance.MoveType_Capture);

            var sfx = DetermineMoveSound(move, isCapture);
            var volume = DetermineMoveVolume(move, isCapture);

            await _soundEffectsModule?.PlaySoundAsync(sfx, volume);
        }


        public async Task HandleEndGameStateAsync(EndGameState gameState)
        {
            var sfx = gameState switch
            {
                EndGameState.Checkmate => SoundEffect.Checkmate,
                EndGameState.StaleMate => SoundEffect.Stalemate,
                _ => SoundEffect.GameEnd
            };

            await _soundEffectsModule?.PlaySoundAsync(sfx, 0.8f);
        }


        public async Task HandleGameStartAsync()
        {
            await _soundEffectsModule?.PlaySoundAsync(SoundEffect.GameStart, 0.6f);
        }


        public async Task HandleGameEndAsync()
        {
            await _soundEffectsModule?.PlaySoundAsync(SoundEffect.GameEnd, 0.7f);
        }


        public async Task HandleUIInteractionAsync(UIInteraction interaction)
        {
            var soundEffect = interaction switch
            {
                UIInteraction.ButtonClick => SoundEffect.ButtonClick,
                UIInteraction.MenuOpen => SoundEffect.MenuOpen,
                UIInteraction.ItemSelected => SoundEffect.ItemSelected,
                UIInteraction.PieceMove => SoundEffect.PieceMove,
                UIInteraction.PieceSelect => SoundEffect.PieceSelected,
                _ => SoundEffect.ButtonClick
            };

            var volume = interaction == UIInteraction.PieceSelect ? 0.3f : 0.5f;
            await _soundEffectsModule?.PlaySoundAsync(soundEffect, volume);
        }


        public void SetSFXEnabled(bool enabled)
        {
            if (_soundEffectsModule != null)
                _soundEffectsModule.IsEnabled = enabled;
        }


        public void SetSFXVolume(float volume)
        {
            _soundEffectsModule?.SetModuleVolume(volume);
        }


        public void SetMasterVolume(float volume)
        {
            _audioEngine.SetMasterVolume(volume);
        }


        private SoundEffect DetermineMoveSound(PossibleMoveInstance move, bool isCapture)
        {
            if (move.type.HasFlag(MoveTypeInstance.MoveType_Checkmate))
                return SoundEffect.Checkmate;
            if (move.type.HasFlag(MoveTypeInstance.MoveType_Check))
                return SoundEffect.Check;
            if (move.type.HasFlag(MoveTypeInstance.MoveType_CastlingKingside) ||
                move.type.HasFlag(MoveTypeInstance.MoveType_CastlingQueenside))
                return SoundEffect.Castling;
            if (move.type.HasFlag(MoveTypeInstance.MoveType_EnPassant))
                return SoundEffect.EnPassant;
            if (move.type.HasFlag(MoveTypeInstance.MoveType_PawnPromotion))
                return SoundEffect.PawnPromotion;
            if (isCapture)
                return SoundEffect.PieceCapture;

            return SoundEffect.PieceMove;
        }


        private float DetermineMoveVolume(PossibleMoveInstance move, bool isCapture)
        {
            if (move.type.HasFlag(MoveTypeInstance.MoveType_Checkmate))
                return 0.9f;
            if (move.type.HasFlag(MoveTypeInstance.MoveType_Check))
                return 0.7f;
            if (isCapture)
                return 0.6f;

            return 0.5f;
        }

    }
}
