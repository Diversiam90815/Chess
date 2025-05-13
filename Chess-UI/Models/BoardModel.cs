using Chess_UI.Services;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Chess_UI.Models
{
    public class BoardModel
    {

        public int[] GetBoardStateFromNative()
        {
            int[] board = new int[64]; // pre-allocated array

            ChessLogicAPI.GetBoardState(board);
            return board;
        }
    }
}
