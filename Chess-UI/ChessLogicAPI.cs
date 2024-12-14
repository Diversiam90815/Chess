using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Chess_UI
{
    public class ChessLogicAPI
    {

        #region DLL Defines

        #region Defines

        private const string LOGIC_API_PATH = @"ChessGame.dll";
        private const int BOARD_SIZE = 8;

        #endregion


        #region Program

        [DllImport(LOGIC_API_PATH, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Init", CharSet = CharSet.Unicode)]
        public static extern void Init();

        [DllImport(LOGIC_API_PATH, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Deinit", CharSet = CharSet.Unicode)]
        public static extern void Deinit();

        [DllImport(LOGIC_API_PATH, CallingConvention = CallingConvention.Cdecl, EntryPoint = "SetDelegate", CharSet = CharSet.Unicode)]
        public static extern void SetDelegate(APIDelegate pDelegate);

        [DllImport(LOGIC_API_PATH, CallingConvention = CallingConvention.Cdecl, EntryPoint = "GetWindowScalingFactor", CharSet = CharSet.Unicode)]
        public static extern float GetWindowScalingFactor(IntPtr hwnd);

        #endregion


        #region Moves

        [DllImport(LOGIC_API_PATH, CallingConvention = CallingConvention.Cdecl, EntryPoint = "GetNumPossibleMoves", CharSet = CharSet.Unicode)]
        public static extern int GetNumPossibleMoves();

        [DllImport(LOGIC_API_PATH, CallingConvention = CallingConvention.Cdecl, EntryPoint = "GetPossibleMoveAtIndex", CharSet = CharSet.Unicode)]
        public static extern bool GetPossibleMoveAtIndex(uint index, out PossibleMoveInstance move);

        [DllImport(LOGIC_API_PATH, CallingConvention = CallingConvention.Cdecl, EntryPoint = "ExecuteMove", CharSet = CharSet.Unicode)]
        public static extern void ExecuteMove(PossibleMoveInstance move);

        #endregion


        #region Board

        [DllImport(LOGIC_API_PATH, CallingConvention = CallingConvention.Cdecl, EntryPoint = "GetPieceInPosition", CharSet = CharSet.Unicode)]
        public static extern void GetPieceInPosition(PositionInstance position, out PieceTypeInstance piece);

        [DllImport(LOGIC_API_PATH, CallingConvention = CallingConvention.Cdecl, EntryPoint = "GetBoardState", CharSet = CharSet.Unicode)]
        public static extern bool GetBoardState(out PieceTypeInstance[] boardState);

        #endregion


        #region Game

        [DllImport(LOGIC_API_PATH, CallingConvention = CallingConvention.Cdecl, EntryPoint = "StartGame", CharSet = CharSet.Unicode)]
        public static extern void StartGame();

        [DllImport(LOGIC_API_PATH, CallingConvention = CallingConvention.Cdecl, EntryPoint = "EndGame", CharSet = CharSet.Unicode)]
        public static extern void EndGame();

        [DllImport(LOGIC_API_PATH, CallingConvention = CallingConvention.Cdecl, EntryPoint = "ResetGame", CharSet = CharSet.Unicode)]
        public static extern void ResetGame();

        #endregion


        #endregion



        #region Delegate

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void APIDelegate(int message, IntPtr data);

        public enum DelegateMessage
        {
            PlayerHasWon = 1,
            InitiateMove
        }

        #endregion




        #region Structures & Enums

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct PositionInstance
        {
            public int x, y;
        }


        public enum PieceTypeInstance
        {
            DefaultType,
            Pawn,
            Knight,
            Bishop,
            Rook,
            Queen,
            King
        }

        public enum PlayerColor
        {
            NoColor,
            White,
            Black
        }

        public enum GameState
        {
            Init = 1,
            OnGoing,
            Paused,
            Checkmate,
            Stalemate,
            Draw
        }

        [Flags]
        public enum MoveTypeInstance : int    // need to set it correctly
        {
            MoveType_None = 0,
            MoveType_Normal = 1 << 0, // 1
            MoveType_DoublePawnPush = 1 << 1, // 2
            MoveType_PawnPromotion = 1 << 2, // 4
            MoveType_Capture = 1 << 3, // 8
            MoveType_EnPassant = 1 << 4, // 16
            MoveType_CastlingKingside = 1 << 5, // 32
            MoveType_CastlingQueenside = 1 << 6, // 64
            MoveType_Check = 1 << 7, // 128
            MoveType_Checkmate = 1 << 8, // 256
        }



        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct PossibleMoveInstance
        {
            public PositionInstance start;
            public PositionInstance end;
            public MoveTypeInstance type;
        }



        #endregion

    }
}
