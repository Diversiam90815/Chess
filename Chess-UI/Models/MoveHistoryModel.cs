using Chess_UI.Services;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Chess_UI.Models
{
    public class MoveHistoryModel
    {
        public List<string> MoveHistory = new();

        public MoveHistoryModel()
        {
            var logicCommunication = App.Current.ChessLogicCommunication as ChessLogicCommunicationLayer;
            logicCommunication.MoveHistoryUpdated += UpdateMoveHistory;
        }


        private void UpdateMoveHistory(string moveNotation)
        {
            MoveHistory.Add(moveNotation);
        }
    }
}
