using static Chess_UI.Services.ChessLogicAPI;

namespace Chess_UI.Services
{
    public static class ChessCoordinate
    {
        public const int BOARD_SIZE = 8;

        /// <summary>
        /// Gets position from a 1D board index (engine coordinates)
        /// </summary>
        public static PositionInstance FromIndex(int index)
        {
            int x = index % BOARD_SIZE;
            int y = index / BOARD_SIZE;
            return new PositionInstance(x, y);
        }

        /// <summary>
        /// Converts from engine coordinates to UI display coordinates
        /// Engine: (0,0) is bottom-left
        /// UI: (0,0) is top-left
        /// </summary>
        public static PositionInstance ToDisplayCoordinates(PositionInstance enginePos)
        {
            return new PositionInstance(enginePos.x, 7 - enginePos.y);
        }

        /// <summary>
        /// Converts from UI display coordinates to engine coordinates
        /// UI: (0,0) is top-left
        /// Engine: (0,0) is bottom-left
        /// </summary>
        public static PositionInstance FromDisplayCoordinates(PositionInstance displayPos)
        {
            return new PositionInstance(displayPos.x, 7 - displayPos.y);
        }

        /// <summary>
        /// Calculates the 1D array index from position
        /// </summary>
        public static int ToIndex(PositionInstance pos, bool forDisplay = false)
        {
            if (forDisplay)
            {
                var displayPos = ToDisplayCoordinates(pos);
                return displayPos.y * BOARD_SIZE + displayPos.x;
            }
            return pos.y * BOARD_SIZE + pos.x;
        }
    }
}
