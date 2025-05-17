using Chess.UI.Services;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Chess.UI.Board
{
    public class BoardModel : IBoardModel
    {
        public int[] GetBoardStateFromNative()
        {
            int[] board = new int[64]; // pre-allocated array

            ChessLogicAPI.GetBoardState(board);
            return board;
        }
    }
}
