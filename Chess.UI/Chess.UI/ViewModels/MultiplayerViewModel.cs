using Chess.UI.Models;
using Chess.UI.Services;
using Microsoft.UI.Dispatching;
using Microsoft.UI.Xaml;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Linq;
using System.Net.Security;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using Windows.Media.Playback;


namespace Chess.UI.ViewModels
{
    public class MultiplayerViewModel : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        private readonly DispatcherQueue DispatcherQueue;

        public readonly MultiplayerModel mModel;

        public MultiplayerMode MPMode { get; private set; }


        public MultiplayerViewModel(DispatcherQueue dispatcher)
        {
            this.DispatcherQueue = dispatcher;
            NetworkAdapters = new();

            mModel = new MultiplayerModel();
            mModel.Init();

            mModel.OnConnectionErrorOccured += HandleConnectionError;
            mModel.OnConnectionStatusChanged += HandleConnectionStatusUpdated;
            mModel.OnClientRequestedConnection += HandleClientRequestConnection;

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
                    mModel.ChangeNetworkAdapter(SelectedAdapter.ID);
                    OnPropertyChanged();
                }
            }
        }


        private bool processing = false;
        public bool Processing
        {
            get => processing;
            set
            {
                if (processing != value)
                {
                    processing = value;
                    SettingsEditable = !value;
                    OnPropertyChanged();
                }
            }
        }


        private bool settingsEditable = true;
        public bool SettingsEditable
        {
            get => settingsEditable;
            set
            {
                if (settingsEditable != value)
                {
                    settingsEditable = value;
                    OnPropertyChanged();
                }
            }
        }


        private bool hostGameButtonEnabled = false;
        public bool HostGameButtonEnabled
        {
            get => hostGameButtonEnabled;
            set
            {
                if (hostGameButtonEnabled != value)
                {
                    hostGameButtonEnabled = value;
                    OnPropertyChanged();
                }
            }
        }


        private bool clientGameButtonEnabled = false;
        public bool ClientGameButtonEnabled
        {
            get => clientGameButtonEnabled;
            set
            {
                if (clientGameButtonEnabled != value)
                {
                    clientGameButtonEnabled = value;
                    OnPropertyChanged();
                }
            }
        }


        private string remotePlayerName;
        public string RemotePlayerName
        {
            get => remotePlayerName;
            set
            {
                if (remotePlayerName != value)
                {
                    remotePlayerName = value;
                    OnPropertyChanged();
                }
            }
        }


        private string localPlayerName;
        public string LocalPlayerName
        {
            get => localPlayerName;
            set
            {
                if (localPlayerName != value)
                {
                    localPlayerName = value;
                    mModel.SetLocalPlayerName(value);
                }
            }
        }


        #region Visibilities

        private Visibility initView = Visibility.Visible;
        public Visibility InitView
        {
            get => initView;
            set
            {
                if (initView != value)
                {
                    initView = value;
                    OnPropertyChanged();
                }
            }
        }


        private Visibility clientFoundHostView = Visibility.Collapsed;
        public Visibility ClientFoundHostView
        {
            get => clientFoundHostView;
            set
            {
                if (clientFoundHostView != value)
                {
                    clientFoundHostView = value;
                    OnPropertyChanged();
                }
            }
        }


        private Visibility clientRequestedConnectionView = Visibility.Collapsed;
        public Visibility ClientRequestedConnectionView
        {
            get => clientRequestedConnectionView;
            set
            {
                if (clientRequestedConnectionView != value)
                {
                    clientRequestedConnectionView = value;
                    OnPropertyChanged();
                }
            }
        }


        private Visibility clientWaitingForResponseView = Visibility.Collapsed;
        public Visibility ClientWaitingForResponseView
        {
            get => clientWaitingForResponseView;
            set
            {
                if (clientWaitingForResponseView != value)
                {
                    clientWaitingForResponseView = value;
                    OnPropertyChanged();
                }
            }
        }

        #endregion


        public void EnterServerMultiplayerMode()
        {
            MPMode = MultiplayerMode.Server;
            mModel.StartGameServer();
            DisplayServerView();
        }


        public void EnterClientMultiplayerMode()
        {
            MPMode = MultiplayerMode.Client;
            mModel.StartGameClient();
            DisplayClientView();
        }


        public void EnterInitMode()
        {
            // TODO:
            // Enter Init Mode Hosting game via Model
            // Setting local player name in UI if saved earlier
            // Display Init view

            mModel.ResetToInit();
            DisplayInitView();
        }


        private void EnterMultiplayerGame()
        {
            if (MPMode == MultiplayerMode.None || MPMode == MultiplayerMode.Init)
                return;

            mModel.StartMultiplerGame(MPMode);
        }


        public void AcceptConnectingToHost()
        {
            //We are the client and accepted a connection to the host
            mModel.ConnectToHost();
        }


        public void DeclineConnectingToHost()
        {
            //We are the client and declined a connection to the host
            EnterInitMode();
        }


        public void AcceptClientConnection()
        {
            //We are the host and accepted a connection try from the client
            mModel.AcceptConnectionRequest();
        }


        public void DeclineClientConnection()
        {
            //We are the host and declined a connection try from the client
            mModel.RejectConnectionRequest();

            EnterInitMode();
        }


        private void HandleConnectionError(string errorMessage)
        {
            // TODO: Display error
        }


        private void HandleConnectionStatusUpdated(ChessLogicAPI.ConnectionState state)
        {
            // TODO: Implement reactive UI
            switch (state)
            {
                case ChessLogicAPI.ConnectionState.HostingSession:
                    {
                        break;
                    }
                case ChessLogicAPI.ConnectionState.Connecting:
                    {
                        break;
                    }
                case ChessLogicAPI.ConnectionState.Connected:
                    {
                        break;
                    }
                case ChessLogicAPI.ConnectionState.WaitingForARemote:
                    {
                        break;
                    }
                case ChessLogicAPI.ConnectionState.Disconnecting:
                    {
                        break;
                    }
                case ChessLogicAPI.ConnectionState.Disconnected:
                    {
                        break;
                    }
                default: break;
            }
        }


        private void HandleClientRequestConnection(string remotePlayerName)
        {
            RemotePlayerName = remotePlayerName;

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


        public void UpdateMPButtons(MultiplayerMode mode)
        {
            switch (mode)
            {
                case MultiplayerMode.None:
                    {
                        HostGameButtonEnabled = false;
                        ClientGameButtonEnabled = false;
                        break;
                    }
                case MultiplayerMode.Init:
                    {
                        HostGameButtonEnabled = true;
                        ClientGameButtonEnabled = true;
                        break;
                    }
                case MultiplayerMode.Server:
                    {
                        HostGameButtonEnabled = true;
                        ClientGameButtonEnabled = false;
                        break;
                    }
                case MultiplayerMode.Client:
                    {
                        HostGameButtonEnabled = false;
                        ClientGameButtonEnabled = true;
                        break;
                    }
                default:
                    {
                        HostGameButtonEnabled = false;
                        ClientGameButtonEnabled = false;
                        break;
                    }
            }
        }


        private void SetLocalPlayerName(string name)
        {
            mModel.SetLocalPlayerName(name);
            SettingsEditable = true;
        }


        public void DisplayClientView()
        {
            InitView = Visibility.Visible;

            Processing = true;
            UpdateMPButtons(MultiplayerMode.Client);

            ClientFoundHostView = Visibility.Collapsed;
            ClientRequestedConnectionView = Visibility.Collapsed;
            ClientWaitingForResponseView = Visibility.Collapsed;
        }


        public void DisplayServerView()
        {
            InitView = Visibility.Visible;

            Processing = true;
            UpdateMPButtons(MultiplayerMode.Server);

            ClientFoundHostView = Visibility.Collapsed;
            ClientRequestedConnectionView = Visibility.Collapsed;
            ClientWaitingForResponseView = Visibility.Collapsed;
        }


        public void DisplayInitView()
        {
            InitView = Visibility.Visible;

            Processing = false;
            UpdateMPButtons(MultiplayerMode.Init);

            ClientFoundHostView = Visibility.Collapsed;
            ClientRequestedConnectionView = Visibility.Collapsed;
            ClientWaitingForResponseView = Visibility.Collapsed;
        }


        public void DisplayClientWaitingForResponseView()
        {
            InitView = Visibility.Collapsed;

            Processing = false;
            UpdateMPButtons(MultiplayerMode.Client);

            ClientFoundHostView = Visibility.Collapsed;
            ClientRequestedConnectionView = Visibility.Collapsed;
            ClientWaitingForResponseView = Visibility.Visible;
        }


        public void DisplayClientFoundHostView()
        {
            InitView = Visibility.Collapsed;

            Processing = false;
            UpdateMPButtons(MultiplayerMode.Server);

            ClientFoundHostView = Visibility.Visible;
            ClientRequestedConnectionView = Visibility.Collapsed;
            ClientWaitingForResponseView = Visibility.Collapsed;
        }


        public void DisplayClientRequestedConnectionView()
        {
            InitView = Visibility.Collapsed;

            Processing = false;
            UpdateMPButtons(MultiplayerMode.Client);

            ClientFoundHostView = Visibility.Collapsed;
            ClientRequestedConnectionView = Visibility.Visible;
            ClientWaitingForResponseView = Visibility.Collapsed;
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
