using System;
using System.Runtime.InteropServices;


namespace Chess_UI.Configuration
{
    public class ChessLogicAPI
    {

        #region DLL Defines

        #region Defines

        private const string LOGIC_API_PATH = @"Chess-Logic.dll";
        public const int BOARD_SIZE = 8;

        #endregion


        #region Program

        [DllImport(LOGIC_API_PATH, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Init", CharSet = CharSet.Unicode)]
        public static extern void Init();

        [DllImport(LOGIC_API_PATH, CallingConvention = CallingConvention.Cdecl, EntryPoint = "Deinit", CharSet = CharSet.Unicode)]
        public static extern void Deinit();

        [DllImport(LOGIC_API_PATH, CallingConvention = CallingConvention.Cdecl, EntryPoint = "SetDelegate", CharSet = CharSet.Unicode)]
        public static extern void SetDelegate(APIDelegate pDelegate);

        [DllImport(LOGIC_API_PATH, CallingConvention = CallingConvention.Cdecl, EntryPoint = "GetWindowScalingFactor", CharSet = CharSet.Unicode)]
        public static extern float GetWindowScalingFactor(nint hwnd);

        [DllImport(LOGIC_API_PATH, CallingConvention = CallingConvention.Cdecl, EntryPoint = "SetUnvirtualizedAppDataPath", CharSet = CharSet.Unicode)]
        public static extern void SetUnvirtualizedAppDataPath([In()][MarshalAs(UnmanagedType.LPStr)] string appDataPath);


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
        public static extern bool GetBoardState([Out, MarshalAs(UnmanagedType.LPArray, SizeConst = 64)] int[] boardState);

        #endregion


        #region Game

        [DllImport(LOGIC_API_PATH, CallingConvention = CallingConvention.Cdecl, EntryPoint = "StartGame", CharSet = CharSet.Unicode)]
        public static extern void StartGame();

        [DllImport(LOGIC_API_PATH, CallingConvention = CallingConvention.Cdecl, EntryPoint = "EndGame", CharSet = CharSet.Unicode)]
        public static extern void EndGame();

        [DllImport(LOGIC_API_PATH, CallingConvention = CallingConvention.Cdecl, EntryPoint = "ResetGame", CharSet = CharSet.Unicode)]
        public static extern void ResetGame();

        [DllImport(LOGIC_API_PATH, CallingConvention = CallingConvention.Cdecl, EntryPoint = "UndoMove", CharSet = CharSet.Unicode)]
        public static extern void UndoMove();

        [DllImport(LOGIC_API_PATH, CallingConvention = CallingConvention.Cdecl, EntryPoint = "ChangeMoveState", CharSet = CharSet.Unicode)]
        public static extern void ChangeMoveState(int state);

        [DllImport(LOGIC_API_PATH, CallingConvention = CallingConvention.Cdecl, EntryPoint = "HandleMoveStateChanged", CharSet = CharSet.Unicode)]
        public static extern void HandleMoveStateChanged(PossibleMoveInstance move);


        #endregion


        #region Logging

        [DllImport(LOGIC_API_PATH, CallingConvention = CallingConvention.Cdecl, EntryPoint = "LogInfoWithCaller", CharSet = CharSet.Unicode)]
        public static extern void LogInfoWithCaller([In()][MarshalAs(UnmanagedType.LPStr)] string message, [In()][MarshalAs(UnmanagedType.LPStr)] string functionName, [In()][MarshalAs(UnmanagedType.LPStr)] string className, int lineNumber);

        [DllImport(LOGIC_API_PATH, CallingConvention = CallingConvention.Cdecl, EntryPoint = "LogErrorWithCaller", CharSet = CharSet.Unicode)]
        public static extern void LogErrorWithCaller([In()][MarshalAs(UnmanagedType.LPStr)] string message, [In()][MarshalAs(UnmanagedType.LPStr)] string functionName, [In()][MarshalAs(UnmanagedType.LPStr)] string className, int lineNumber);

        [DllImport(LOGIC_API_PATH, CallingConvention = CallingConvention.Cdecl, EntryPoint = "LogWarningWithCaller", CharSet = CharSet.Unicode)]
        public static extern void LogWarningWithCaller([In()][MarshalAs(UnmanagedType.LPStr)] string message, [In()][MarshalAs(UnmanagedType.LPStr)] string functionName, [In()][MarshalAs(UnmanagedType.LPStr)] string className, int lineNumber);


        #endregion


        #endregion



        #region Delegate

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void APIDelegate(int message, nint data);

        public enum DelegateMessage
        {
            PlayerHasWon = 1,
            InitiateMove,
            PlayerScoreUpdate,
            PlayerCapturedPiece,
            MoveExecuted,
            PlayerChanged,
            GameStateChanged,
            MoveHistoryAdded
        }

        #endregion



        #region Structures & Enums

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct PositionInstance
        {
            public PositionInstance(int x, int y)
            {
                this.x = x;
                this.y = y;
            }

            public int x { get; set; }
            public int y { get; set; }


            public override readonly bool Equals(object obj)
            {
                if (obj is PositionInstance)
                {
                    var other = (PositionInstance)obj;
                    return (this.x == other.x) && (this.y == other.y);
                }
                return false;
            }


            public static bool operator ==(PositionInstance left, PositionInstance right)
            {
                if (Equals(left, right))
                {
                    return true;
                }

                return left.Equals(right);
            }


            public static bool operator !=(PositionInstance left, PositionInstance right)
            {
                return !(left == right);
            }


            public override int GetHashCode()
            {
                int hashcode = 17;
                hashcode = hashcode * 23 + x.GetHashCode();
                hashcode = hashcode * 23 + y.GetHashCode();
                return hashcode;
            }
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


        public enum MoveState
        {
            NoMove = 1,
            InitiateMove,
            ExecuteMove
        }


        [Flags]
        public enum MoveTypeInstance : int
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
            public PieceTypeInstance promotionPiece;

            public override readonly bool Equals(object obj)
            {
                if (obj is PossibleMoveInstance)
                {
                    var other = (PossibleMoveInstance)obj;
                    return (start == other.start) && (end == other.end);
                }
                return false;
            }


            public static bool operator ==(PossibleMoveInstance left, PossibleMoveInstance right)
            {
                if (Equals(left, right))
                {
                    return true;
                }

                return left.Equals(right);
            }


            public static bool operator !=(PossibleMoveInstance left, PossibleMoveInstance right)
            {
                return !left.Equals(right);
            }


            public override int GetHashCode()
            {
                return start.GetHashCode() ^ end.GetHashCode();
            }
        }


        [StructLayout(LayoutKind.Sequential)]
        public struct Score
        {
            public PlayerColor player;
            public int score;
        }


        [StructLayout(LayoutKind.Sequential)]
        public struct PlayerCapturedPiece
        {
            public PlayerColor playerColor;
            public PieceTypeInstance pieceType;
        };


        #endregion

    }
}
