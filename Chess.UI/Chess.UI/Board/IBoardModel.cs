using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


namespace Chess_UI.Board
{
    public interface IBoardModel
    {
        int[] GetBoardStateFromNative();
    }
}