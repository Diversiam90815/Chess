using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Media.Imaging;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static Chess_UI.Configuration.ChessLogicAPI;


namespace Chess_UI.Configuration
{
    public static class Images
    {
        public enum MainMenuButton
        {
            StartGame = 1,
            Settings,
            Multiplayer,
            EndGame
        }

        public enum BoardBackground
        {
            Wood = 1
        }


        public static readonly Dictionary<BoardBackground, ImageSource> BoardBackgroundImages = new()
        {
        {BoardBackground.Wood, LoadImage("/Assets/Board/wood.png") }
        };

        public static readonly Dictionary<MainMenuButton, ImageSource> MainMenutImages = new()
        {
        {MainMenuButton.StartGame, LoadImage("/Assets/Buttons/pawn.png") },
        {MainMenuButton.Settings, LoadImage("/Assets/Buttons/rook.png") },
        {MainMenuButton.Multiplayer, LoadImage("/Assets/Buttons/queen.png") },
        {MainMenuButton.EndGame, LoadImage("/Assets/Buttons/horse.png") }
        };

        public static readonly Dictionary<PieceTypeInstance, ImageSource> CapturedWhitePiecesImages = new()
        {
        {PieceTypeInstance.Pawn, LoadImage("/Assets/Pieces/Standard/PawnW.png") },
        {PieceTypeInstance.Bishop, LoadImage("/Assets/Pieces/Standard/BishopW.png") },
        {PieceTypeInstance.Queen, LoadImage("/Assets/Pieces/Standard/QueenW.png") },
        {PieceTypeInstance.Rook, LoadImage("/Assets/Pieces/Standard/RookW.png") },
        {PieceTypeInstance.Knight, LoadImage("/Assets/Pieces/Standard/KnightW.png") },
        {PieceTypeInstance.King, LoadImage("/Assets/Pieces/Standard/KingW.png") }
        };

        public static readonly Dictionary<PieceTypeInstance, ImageSource> CapturedBlackPiecesImages = new()
        {
        {PieceTypeInstance.Pawn, LoadImage("/Assets/Pieces/Standard/PawnB.png") },
        {PieceTypeInstance.Bishop, LoadImage("/Assets/Pieces/Standard/BishopB.png") },
        {PieceTypeInstance.Queen, LoadImage("/Assets/Pieces/Standard/QueenB.png") },
        {PieceTypeInstance.Rook, LoadImage("/Assets/Pieces/Standard/RookB.png") },
        {PieceTypeInstance.Knight, LoadImage("/Assets/Pieces/Standard/KnightB.png") },
        {PieceTypeInstance.King, LoadImage("/Assets/Pieces/Standard/KingB.png") }
        };



        public static ImageSource LoadImage(string relativeFilePath)
        {
            return new BitmapImage(new Uri($"ms-appx:///{relativeFilePath.TrimStart('/')}"));
        }


        public static ImageSource GetImage(MainMenuButton button) => MainMenutImages[button];

        public static ImageSource GetImage(BoardBackground background) => BoardBackgroundImages[background];

        public static ImageSource GetCapturedPieceImage(PlayerColor player, PieceTypeInstance pieceTypeInstance)
        {
            return player switch
            {
                PlayerColor.White => CapturedWhitePiecesImages[pieceTypeInstance],
                PlayerColor.Black => CapturedBlackPiecesImages[pieceTypeInstance],
                _ => null
            };
        }


    }
}
