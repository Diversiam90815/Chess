using Chess.UI.Services;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace Chess.UI.Models
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
            MoveHistoryUpdated?.Invoke();
        }


        public void RemoveLastMove()
        {
            MoveHistory.Remove(MoveHistory.LastOrDefault());
        }


        public delegate void MoveHistoryUpdatedHandler();
        public event MoveHistoryUpdatedHandler MoveHistoryUpdated;
    }
}
