using Chess.UI.Multiplayer;
using Chess.UI.Wrappers;
using Microsoft.Extensions.DependencyInjection;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Runtime.CompilerServices;


namespace Chess.UI.ViewModels
{
    public partial class MultiplayerPreferencesViewModel : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        private readonly IDispatcherQueueWrapper _dispatcherQueue;

        private readonly IMultiplayerPreferencesModel _model;


        public MultiplayerPreferencesViewModel(IDispatcherQueueWrapper dispatcherQueue)
        {
            _dispatcherQueue = dispatcherQueue;

            NetworkAdapters = [];

            _model = App.Current.Services.GetService<IMultiplayerPreferencesModel>();
            _model.Init();

            UpdateAdapterBox();
            SetLocalNameFromBackend();
        }


        #region Network

        private ObservableCollection<NetworkAdapter> _networkAdapters;
        public ObservableCollection<NetworkAdapter> NetworkAdapters
        {
            get => _networkAdapters;
            set
            {
                if (_networkAdapters != value)
                {
                    _networkAdapters = value;
                    OnPropertyChanged();
                }
            }
        }


        private NetworkAdapter _selectedAdapter;
        public NetworkAdapter SelectedAdapter
        {
            get => _selectedAdapter;
            set
            {
                if (_selectedAdapter != value)
                {
                    _selectedAdapter = value;
                    _model.ChangeNetworkAdapter(SelectedAdapter.ID);
                    OnPropertyChanged();
                }
            }
        }


        public void SelectPresavedNetworkAdapter()
        {
            int savedAdapterID = _model.GetSelectedNetworkAdapterID();

            if (savedAdapterID != 0)
            {
                for (int i = 0; i < NetworkAdapters.Count; i++)
                {
                    Multiplayer.NetworkAdapter adapter = NetworkAdapters[i];
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
            _dispatcherQueue.TryEnqueue(() =>
            {
                NetworkAdapters.Clear();
                var adapters = _model.GetNetworkAdapters();

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
        #endregion


        #region Player Name


        private string _localPlayerName;
        public string LocalPlayerName
        {
            get => _localPlayerName;
            set
            {
                if (_localPlayerName != value)
                {
                    _localPlayerName = value;
                    _model.SetLocalPlayerName(value);
                    OnPropertyChanged();
                }
            }
        }


        private void SetLocalNameFromBackend()
        {
            LocalPlayerName = _model.GetLocalPlayerName();
        }

        #endregion


        protected void OnPropertyChanged([CallerMemberName] string name = null)
        {
            _dispatcherQueue.TryEnqueue(() =>
            {
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
            });
        }
    }
}
