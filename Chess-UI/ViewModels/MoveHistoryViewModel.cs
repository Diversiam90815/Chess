using Chess_UI.MoveHistory;
using Chess_UI.Services;
using Chess_UI.Wrappers;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.UI.Dispatching;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;


namespace Chess_UI.ViewModels
{
    public class MoveHistoryViewModel
    {
        private const int MovesMaxColumns = 3;

        public ObservableCollection<ObservableCollection<string>> MoveHistoryColumns { get; } = [];

        private IMoveHistoryModel _model { get; }

        private readonly IDispatcherQueueWrapper _dispatcherQueue;


        public MoveHistoryViewModel(IDispatcherQueueWrapper dispatcher)
        {
            _dispatcherQueue = dispatcher;
            _model = App.Current.Services.GetService<IMoveHistoryModel>();

            for (int i = 0; i < MovesMaxColumns; i++)
            {
                MoveHistoryColumns.Add(new ObservableCollection<string>());
            }
            _model.MoveHistoryUpdated += OnHandleMoveHistoryUpdated;
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
            _model.RemoveLastMove();
            OnHandleMoveHistoryUpdated();
        }


        private void OnHandleMoveHistoryUpdated()
        {
            _dispatcherQueue.TryEnqueue(() =>
            {
                ClearMoveHistory();

                foreach (var moveNotation in _model.MoveHistory)
                {
                    AddMove(moveNotation);
                }
            });
        }
    }
}
