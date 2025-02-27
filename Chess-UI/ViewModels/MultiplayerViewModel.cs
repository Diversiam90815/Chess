using Chess_UI.Models;
using Microsoft.UI.Dispatching;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;


namespace Chess_UI.ViewModels
{
    public class MultiplayerViewModel : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        private readonly DispatcherQueue DispatcherQueue;

        public readonly MultiplayerModel mModel;


        public MultiplayerViewModel(DispatcherQueue dispatcher)
        {
            this.DispatcherQueue = dispatcher;
            NetworkAdapters = new();

            mModel = new MultiplayerModel();
            mModel.Init();

            UpdateAdapterBox();
        }


        private ObservableCollection<NetworkAdapter> networkAdapters;
        public ObservableCollection<NetworkAdapter> NetworkAdapters
        {
            get => networkAdapters;
            set
            {
                if (networkAdapters != value)
                {
                    networkAdapters = value;
                    OnPropertyChanged();
                }
            }
        }


        private NetworkAdapter selectedAdapter;
        public NetworkAdapter SelectedAdapter
        {
            get => selectedAdapter;
            set
            {
                if (selectedAdapter != value)
                {
                    selectedAdapter = value;
                    OnPropertyChanged();
                }
            }
        }


        public void SelectPresavedNetworkAdapter()
        {
            int savedAdapterID = mModel.GetSelectedNetworkAdapterID();

            if (savedAdapterID != 0)
            {
                for (int i = 0; i < NetworkAdapters.Count; i++)
                {
                    NetworkAdapter adapter = NetworkAdapters[i];
                    if (adapter != null && adapter.ID == savedAdapterID)
                    {
                        SelectedAdapter = adapter;
                        return;
                    }
                }
            }
        }


        public void UpdateAdapterBox()
        {
            DispatcherQueue.TryEnqueue(() =>
            {
                NetworkAdapters.Clear();
                var adapters = mModel.GetNetworkAdapters();

                foreach (var adapter in adapters)
                {
                    if (!NetworkAdapters.Contains(adapter))
                    {
                        NetworkAdapters.Add(adapter);
                    }
                }
                SelectPresavedNetworkAdapter();
            });
        }


        protected void OnPropertyChanged([CallerMemberName] string name = null)
        {
            DispatcherQueue.TryEnqueue(() =>
            {
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
            });
        }
    }
}
