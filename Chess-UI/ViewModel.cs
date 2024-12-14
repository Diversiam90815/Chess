using Microsoft.UI.Dispatching;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace Chess_UI
{
    public class ViewModel : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        private readonly DispatcherQueue dispatcherQueue;

        private Controller mController;


        public ViewModel(DispatcherQueue dispatcher)
        {
            mController = new Controller();
            dispatcherQueue = dispatcher;
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
