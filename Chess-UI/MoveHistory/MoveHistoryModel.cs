using Chess_UI.Services;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace Chess_UI.MoveHistory
{
    public class MoveHistoryModel : IMoveHistoryModel
    {
        public List<string> MoveHistory { get; } = new();

        public MoveHistoryModel()
        {
            var logicCommunication = App.Current.ChessLogicCommunication as CommunicationLayer;
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

        public event Action MoveHistoryUpdated;
    }
}
