using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static Chess.UI.Services.ChessLogicAPI;

namespace Chess.UI.Services.Interfaces
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