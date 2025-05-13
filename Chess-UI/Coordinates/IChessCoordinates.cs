using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static Chess_UI.Services.ChessLogicAPI;

namespace Chess_UI.Services.Interfaces
{
    public interface IChessCoordinate
    {
        int GetNumBoardSquares();
        PositionInstance FromIndex(int index);
        int ToIndex(PositionInstance pos, bool forDisplay = false);
        PositionInstance ToDisplayCoordinates(PositionInstance enginePos);
        PositionInstance FromDisplayCoordinates(PositionInstance displayPos);
    }
}