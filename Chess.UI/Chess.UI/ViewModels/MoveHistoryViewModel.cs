using Chess.UI.Models;
using Chess.UI.Services;
using Microsoft.UI.Dispatching;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;


namespace Chess.UI.ViewModels
{
    public class MoveHistoryViewModel
    {
        private const int MovesMaxColumns = 3;

        public ObservableCollection<ObservableCollection<string>> MoveHistoryColumns { get; } = [];

        public MoveHistoryModel Model { get; } = new MoveHistoryModel();

        private readonly Microsoft.UI.Dispatching.DispatcherQueue DispatcherQueue;


        public MoveHistoryViewModel(DispatcherQueue dispatcher)
        {
            DispatcherQueue = dispatcher;

            for (int i = 0; i < MovesMaxColumns; i++)
            {
                MoveHistoryColumns.Add(new ObservableCollection<string>());
            }
            Model.MoveHistoryUpdated += OnHandleMoveHistoryUpdated;
        }


        public void AddMove(string move)
        {
            // Find the column with the least number of moves
            var minColumn = MoveHistoryColumns.OrderBy(col => col.Count).First();

            minColumn.Add(move);
        }


        public void ClearMoveHistory()
        {
            foreach (var column in MoveHistoryColumns)
            {
                column.Clear();
            }
        }


        public void RemoveLastMove()
        {
            Model.RemoveLastMove();
            OnHandleMoveHistoryUpdated();
        }


        private void OnHandleMoveHistoryUpdated()
        {
            DispatcherQueue.TryEnqueue(() =>
            {
                ClearMoveHistory();

                foreach (var moveNotation in Model.MoveHistory)
                {
                    AddMove(moveNotation);
                }
            });
        }
    }
}
