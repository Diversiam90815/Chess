/*
  ==============================================================================
	Module:         CPU Performance Tests
	Description:    Testing performance through CPU vs CPU games
  ==============================================================================
*/

#include <gtest/gtest.h>
#include <chrono>
#include <memory>

#include "GameEngine/GameEngine.h"


namespace PerformanceTests
{

struct GameResult
{
	PlayerColor				  winner	= PlayerColor::NoColor;
	EndGameState			  endState	= EndGameState::OnGoing;
	int						  moveCount = 0;
	std::chrono::milliseconds duration{0};
	std::string				  gameDescription;
};



struct CPUPerformanceStats
{
	int						  wins		 = 0;
	int						  losses	 = 0;
	int						  draws		 = 0;
	int						  totalGames = 0;
	double					  winRate	 = 0.0;
	std::chrono::milliseconds averageGameDuration{0};
	int						  averageMoveCount = 0;

	void					  calculateStats()
	{
		if (totalGames > 0)
		{
			winRate = static_cast<double>(wins) / totalGames * 100;
		}
	}
};


class CPUPerformanceTests : public ::testing::Test
{
protected:
	std::shared_ptr<GameEngine>			  mEngine;

	std::atomic<bool>					  mGameCompleted{false};
	std::atomic<bool>					  mGameRunning{false};
	GameResult							  mCurrentGameResult;
	std::chrono::steady_clock::time_point mGameStartTime;
};



} // namespace PerformanceTests