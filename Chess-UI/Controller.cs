using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static Chess_UI.ChessLogicAPI;

namespace Chess_UI
{
    public class Controller
    {
        public Controller()
        {

        }

        static APIDelegate mDelegate = null;

        private void SetLogicAPIDelegate()
        {
            if (mDelegate == null)
            {
                mDelegate = new APIDelegate(DelegateHandler);
                ChessLogicAPI.SetDelegate(mDelegate);
            }


        }

        public void DelegateHandler(int message, IntPtr data)
        {
            DelegateMessage delegateMessage = (DelegateMessage)message;
            switch (delegateMessage)
            {
                case DelegateMessage.NotYetImplemented:
                    {
                        break;
                    }

                default: break;
            }
        }

    }
}
