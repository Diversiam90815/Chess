using static Chess.UI.Services.EngineAPI;


namespace Chess.UI.Services
{
    public class GameConfigurationBuilder
    {
        private GameConfiguration _config;


        public GameConfigurationBuilder()
        {
            Reset();
        }


        public void Reset()
        {
            _config = new GameConfiguration
            {
                Mode = GameModeSelection.None,
                PlayerColor = PlayerColor.White,
                CpuDifficulty = 0
            };
        }


        public void SetGameMode(GameModeSelection mode)
        {
            _config.Mode = mode;
        }


        public void SetPlayerColor(PlayerColor color)
        {
            _config.PlayerColor = color;
        }


        public void SetCpuDifficulty(CPUDifficulty difficulty)
        {
            _config.CpuDifficulty = difficulty;
        }


        public GameConfiguration GetConfiguration()
        {
            return _config;
        }


        public GameConfiguration GetCancelledConfiguration()
        {
            var cancelConfig = _config;
            cancelConfig.Mode = GameModeSelection.None;
            return cancelConfig;
        }
    }
}
