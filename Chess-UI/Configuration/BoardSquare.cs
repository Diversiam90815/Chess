using Microsoft.UI.Xaml.Media;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static Chess_UI.Configuration.ChessLogicAPI;

namespace Chess_UI.Configuration
{
    public class BoardSquare
    {
        public BoardSquare()
        {
            this.pos.x = 0;
            this.pos.y = 0;
            this.piece = PieceTypeInstance.DefaultType;
            this.colour = PlayerColor.NoColor;
        }

        public BoardSquare(int x, int y, PieceTypeInstance pieceTypeInstance, PlayerColor color)
        {
            this.pos.x = x;
            this.pos.y = y;
            this.piece = pieceTypeInstance;
            this.colour = color;
        }


        public PositionInstance pos;

        public PieceTypeInstance piece;

        public PlayerColor colour;

        public ImageSource PieceImage
        {
            get
            {
                if (piece == PieceTypeInstance.DefaultType || colour == PlayerColor.NoColor)
                    return null;

                return Images.GetCapturedPieceImage(colour, piece);
            }
        }
    }
}
