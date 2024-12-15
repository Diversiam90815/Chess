using Microsoft.UI.Dispatching;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace Chess_UI.ViewModels
{
    public class ChessBoardViewModel : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        private readonly DispatcherQueue dispatcherQueue;

        
        public ChessBoardViewModel(DispatcherQueue dispatcherQueue)
        {
            this.dispatcherQueue = dispatcherQueue;
        }


        protected void OnPropertyChanged([CallerMemberName] string name = null)
        {
            dispatcherQueue.TryEnqueue(() =>
            {
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
            });
        }
    }
}
