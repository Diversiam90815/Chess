using Chess.UI.Services;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Chess.UI.Multiplayer
{
    public record NetworkAdapter
    {
        public string Name { get; set; }
        public int ID { get; set; }
    }


    public class MultiplayerPreferencesModel : IMultiplayerPreferencesModel
    {
        private readonly List<NetworkAdapter> _adapters = [];


        public void Init()
        {
            SetNetworkAdapters();
        }


        #region Network

        private bool SetNetworkAdapters()
        {
            _adapters.Clear();
            int adapterCount = EngineAPI.GetNetworkAdapterCount();

            for (uint i = 0; i < adapterCount; ++i)
            {
                EngineAPI.GetNetworkAdapterAtIndex(i, out EngineAPI.NetworkAdapter adapter);

                NetworkAdapter networkAdapter = new()
                {
                    Name = adapter.name,
                    ID = adapter.id
                };

                if (networkAdapter.ID == 0 && networkAdapter.Name == null)
                    continue;

                _adapters.Add(networkAdapter);
            }

            bool result = _adapters.Count > 0;
            return result;
        }


        public List<NetworkAdapter> GetNetworkAdapters()
        {
            return _adapters;
        }


        public int GetSelectedNetworkAdapterID()
        {
            return EngineAPI.GetSavedAdapterID();
        }


        public void ChangeNetworkAdapter(int ID)
        {
            EngineAPI.ChangeCurrentAdapter(ID);
        }

        #endregion


        #region Player Name

        public void SetLocalPlayerName(string name)
        {
            EngineAPI.SetLocalPlayerName(name);
            PlayerNameChanged?.Invoke(name);
        }


        public string GetLocalPlayerName()
        {
            string localPlayerName = EngineAPI.GetLocalPlayerName();
            return localPlayerName;
        }

        #endregion

        public event Action<string> PlayerNameChanged;

    }
}
