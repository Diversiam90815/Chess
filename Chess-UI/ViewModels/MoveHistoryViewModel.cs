using Chess_UI.Models;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


namespace Chess_UI.ViewModels
{
    public class MoveHistoryViewModel
    {

        private const int MovesMaxColumns = 3;

        public ObservableCollection<ObservableCollection<string>> MoveHistoryColumns { get; } = [];

        public MoveHistoryModel Model { get; } = new MoveHistoryModel();



        public MoveHistoryViewModel()
        {
            for (int i = 0; i < MovesMaxColumns; i++)
            {
                MoveHistoryColumns.Add(new ObservableCollection<string>());
            }
        }


        public void AddMove(string move)
        {
            // Find the column with the least number of moves
            var minColumn = MoveHistoryColumns.OrderBy(col => col.Count).First();
            minColumn.Add(move);
        }


        public void ClearMoveHistory()
        {
            MoveHistoryColumns.Clear();
            for (int i = 0; i < MovesMaxColumns; i++)
            {
                MoveHistoryColumns.Add(new ObservableCollection<string>());
            }
        }


        public void RemoveLastMove()
        {
            Model.MoveHistory.Remove(Model.MoveHistory.LastOrDefault());
            OnHandleMoveHistoryUpdated();
        }


        private void OnHandleMoveHistoryUpdated()
        {
            ClearMoveHistory();

            foreach (var moveNotation in Model.MoveHistory)
            {
                AddMove(moveNotation);
            }
        }

    }
}
