using Chess_UI.Services;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Chess_UI.Models
{
    public class NetworkAdapter
    {
        public string Name { get; set; }
        public int ID { get; set; }
    }


    public class MultiplayerModel
    {
        private List<NetworkAdapter> mAdapters = new();


        public void Init()
        {
            SetNetworkAdapters();
        }


        private bool SetNetworkAdapters()
        {
            mAdapters.Clear();
            int adapterCount = ChessLogicAPI.GetNetworkAdapterCount();

            for (uint i = 0; i < adapterCount; ++i)
            {
                ChessLogicAPI.NetworkAdapter adapter;
                ChessLogicAPI.GetNetworkAdapterAtIndex(i, out adapter);

                NetworkAdapter networkAdapter = new();
                networkAdapter.Name = adapter.name;
                networkAdapter.ID = adapter.id;

                if (networkAdapter.ID == 0 && networkAdapter.Name == null)
                    continue;

                mAdapters.Add(networkAdapter);
            }

            bool result = mAdapters.Count > 0;
            return result;
        }


        public List<NetworkAdapter> GetNetworkAdapters()
        {
            return mAdapters;
        }


        public int GetSelectedNetworkAdapterID()
        {
            return ChessLogicAPI.GetSavedAdapterID();
        }


        public void ChangeNetworkAdapter(int ID)
        {
            ChessLogicAPI.ChangeCurrentAdapter(ID);
        }


        public string GetRemotePlayerName()
        {
            return ChessLogicAPI.GetRemotePlayerName();
        }


        public void SetLocalPlayerName(string name)
        {
            ChessLogicAPI.SetLocalPlayerName(name);
        }
    }
}
