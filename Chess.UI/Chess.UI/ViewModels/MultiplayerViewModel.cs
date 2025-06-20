using Chess.UI.Models;
using Chess.UI.Multiplayer;
using Chess.UI.Services;
using Chess.UI.Wrappers;
using Microsoft.Extensions.DependencyInjection;
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

        private readonly IDispatcherQueueWrapper _dispatcherQueue;

        private readonly IMultiplayerModel _model;

        public MultiplayerMode MPMode { get; private set; }

        public event Action RequestNavigationToChessboard;


        public MultiplayerViewModel(IDispatcherQueueWrapper dispatcher)
        {
            _dispatcherQueue = dispatcher;

            NetworkAdapters = new();

            _model = App.Current.Services.GetService<IMultiplayerModel>();
            _model.Init();

            _model.OnConnectionErrorOccured += HandleConnectionError;
            _model.OnConnectionStatusChanged += HandleConnectionStatusUpdated;
            _model.OnPlayerChanged += HandlePlayerChanged;

            //UpdateAdapterBox();
        }


        public void StartMultiplayerSetup()
        {
            _model.StartMultiplayer();
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
                    _model.ChangeNetworkAdapter(SelectedAdapter.ID);
                    OnPropertyChanged();
                }
            }
        }


        private bool isLocalPlayersTurn = false;
        public bool IsLocalPlayersTurn
        {
            get => isLocalPlayersTurn;
            set
            {
                if (isLocalPlayersTurn != value)
                {
                    isLocalPlayersTurn = value;
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


        private EngineAPI.PlayerColor localPlayer;
        public EngineAPI.PlayerColor LocalPlayer
        {
            get => localPlayer;
            set
            {
                if (localPlayer != value)
                {
                    localPlayer = value;
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
                    _model.SetLocalPlayerName(value);
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


        public void HandlePlayerChanged(Services.EngineAPI.PlayerColor player)
        {
            IsLocalPlayersTurn = player == LocalPlayer;
        }


        public void EnterServerMultiplayerMode()
        {
            MPMode = MultiplayerMode.Server;
            _model.StartGameServer();
            DisplayServerView();
        }


        public void EnterClientMultiplayerMode()
        {
            MPMode = MultiplayerMode.Client;
            _model.StartGameClient();
            DisplayClientView();
        }


        public void EnterInitMode()
        {
            // TODO:
            // Enter Init Mode Hosting game via Model
            // Setting local player name in UI if saved earlier
            // Display Init view

            _model.ResetToInit();
            DisplayInitView();
        }


        private void EnterMultiplayerGame()
        {
            if (MPMode == MultiplayerMode.None || MPMode == MultiplayerMode.Init)
                return;

            LocalPlayer = MPMode == MultiplayerMode.Server ? EngineAPI.PlayerColor.White : EngineAPI.PlayerColor.White;     // Server / Host mode starts with white for now

            _model.StartMultiplerGame(MPMode);
        }


        public void AcceptConnectingToHost()
        {
            //We are the client and accepted a connection to the host
            _model.ConnectToHost();
        }


        public void DeclineConnectingToHost()
        {
            //We are the client and declined a connection to the host
            EnterInitMode();
        }


        public void AcceptClientConnection()
        {
            //We are the host and accepted a connection try from the client
            _model.AcceptConnectionRequest();
        }


        public void DeclineClientConnection()
        {
            //We are the host and declined a connection try from the client
            _model.RejectConnectionRequest();

            EnterInitMode();
        }


        private void HandleConnectionError(string errorMessage)
        {
            // TODO: Display error
        }


        private void HandleConnectionStatusUpdated(EngineAPI.ConnectionState state, string remotePlayerName)
        {
            // TODO: Implement reactive UI
            switch (state)
            {
                case EngineAPI.ConnectionState.HostingSession:
                    {
                        break;
                    }
                case EngineAPI.ConnectionState.ConnectionRequested:
                    {
                        RemotePlayerName = remotePlayerName;
                        DisplayClientRequestedConnectionView();
                        break;
                    }
                case EngineAPI.ConnectionState.Connecting:
                    {
                        break;
                    }
                case EngineAPI.ConnectionState.Connected:
                    {
                        // Start the multiplayer game
                        EnterMultiplayerGame();

                        // Navigate to chessboard window
                        RequestNavigationToChessboard?.Invoke();

                        break;
                    }
                case EngineAPI.ConnectionState.WaitingForARemote:
                    {
                        break;
                    }
                case EngineAPI.ConnectionState.Disconnecting:
                    {
                        break;
                    }
                case EngineAPI.ConnectionState.Disconnected:
                    {
                        break;
                    }
                case EngineAPI.ConnectionState.ClientFoundHost:
                    {
                        RemotePlayerName = remotePlayerName;
                        DisplayClientFoundHostView();
                        break;
                    }
                default: break;
            }
        }


        public void SelectPresavedNetworkAdapter()
        {
            int savedAdapterID = _model.GetSelectedNetworkAdapterID();

            if (savedAdapterID != 0)
            {
                for (int i = 0; i < NetworkAdapters.Count; i++)
                {
                    Models.NetworkAdapter adapter = NetworkAdapters[i];
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
            _model.SetLocalPlayerName(name);
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
            _dispatcherQueue.TryEnqueue(() =>
            {
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
            });
        }
    }
}
