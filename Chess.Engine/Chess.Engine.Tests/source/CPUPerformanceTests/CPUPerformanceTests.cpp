/*
  ==============================================================================
	Module:         CPU Performance Tests
	Description:    Testing performance through CPU vs CPU games
  ==============================================================================
*/

#include <gtest/gtest.h>
#include <chrono>
#include <map>
#include <vector>
#include <thread>
#include <atomic>
#include <fstream>
#include <iomanip>
#include <memory>

#include "GameEngine/GameEngine.h"
#include "StateMachine/StateMachine.h"
#include "CPUPLayer/CPUPlayer.h"
#include "Moves/Evaluation/MoveEvaluation.h"


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


class MoveEvaluationPerformanceTests : public ::testing::Test, public IGameObserver, public IGameStateObserver, public std::enable_shared_from_this<MoveEvaluationPerformanceTests>
{
protected:
	std::shared_ptr<GameEngine>			  mGameEngine;
	std::shared_ptr<StateMachine>		  mStateMachine;

	std::atomic<bool>					  mGameCompleted{false};
	std::atomic<bool>					  mGameRunning{false};
	GameResult							  mCurrentGameResult;
	std::chrono::steady_clock::time_point mGameStartTime;


	void								  SetUp() override
	{
		mGameEngine = std::make_shared<GameEngine>();
		mGameEngine->init();
		mGameEngine->attachObserver(shared_from_this());

		mStateMachine = StateMachine::GetInstance();
		mStateMachine->attachObserver(shared_from_this());
	}


	void TearDown() override
	{
		mGameEngine->detachObserver(shared_from_this());
		mStateMachine->detachObserver(shared_from_this());
		StateMachine::ReleaseInstance();
	}


	// IGameObserver implementation
	void onEndGame(EndGameState state, PlayerColor winner) override
	{
		auto endTime				= std::chrono::steady_clock::now();
		mCurrentGameResult.winner	= winner;
		mCurrentGameResult.endState = state;
		mCurrentGameResult.duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - mGameStartTime);
		mGameCompleted.store(true);
		mGameRunning.store(false);
	}


	void onChangeCurrentPlayer(PlayerColor player) override
	{
		// Count moves
		mCurrentGameResult.moveCount++;
	}


	// IGameStateObserver implementation
	void onGameStateChanged(GameState state) override
	{
		if (state == GameState::Init)
		{
			mGameStartTime = std::chrono::steady_clock::now();
			mGameRunning.store(true);
			mGameCompleted.store(false);
			mCurrentGameResult = GameResult{};
		}
	}


	GameResult runSingleCPUvsCPUGame(const CPUConfiguration &whiteCPU,
									 const CPUConfiguration &blackCPU,
									 const std::string		&description = "",
									 int					 maxMoves	 = 200,
									 std::chrono::seconds	 timeout	 = std::chrono::seconds(300))
	{
		mCurrentGameResult.gameDescription = description;

		// Reset game state
		mGameEngine->resetGame();
		mStateMachine->resetGame();

		// Configure CPU players
		mGameEngine->setWhiteCPUConfiguration(whiteCPU);
		mGameEngine->setBlackCPUConfiguration(blackCPU);

		// Start CPU vs CPU game
		mStateMachine->onCPUvsCPUGameStarted(whiteCPU, blackCPU);

		// Wait for game completion with timeout
		auto startTime = std::chrono::steady_clock::now();
		while (mGameRunning.load() && std::chrono::steady_clock::now() - startTime < timeout && mCurrentGameResult.moveCount < maxMoves)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		if (mGameRunning.load())
		{
			// Game timed out or exceeded max moves
			mCurrentGameResult.endState = EndGameState::StaleMate;
			mCurrentGameResult.winner	= PlayerColor::NoColor;
			mGameRunning.store(false);
		}

		return mCurrentGameResult;
	}


	CPUPerformanceStats runMultipleGames(const CPUConfiguration &testCPU, const CPUConfiguration &referenceCPU, int gameCount, bool testCPUAsWhite = true)
	{
		CPUPerformanceStats		stats;
		std::vector<GameResult> games;

		for (int i = 0; i < gameCount; ++i)
		{
			CPUConfiguration whiteCPU = testCPUAsWhite ? testCPU : referenceCPU;
			CPUConfiguration blackCPU = testCPUAsWhite ? referenceCPU : testCPU;

			whiteCPU.cpuColor		  = PlayerColor::White;
			blackCPU.cpuColor		  = PlayerColor::Black;

			std::string description	  = "Game " + std::to_string(i + 1) + "/" + std::to_string(gameCount);
			GameResult	result		  = runSingleCPUvsCPUGame(whiteCPU, blackCPU, description);

			games.push_back(result);

			// Update stats
			stats.totalGames++;
			if (result.winner == (testCPUAsWhite ? PlayerColor::White : PlayerColor::Black))
			{
				stats.wins++;
			}
			else if (result.winner == (testCPUAsWhite ? PlayerColor::Black : PlayerColor::White))
			{
				stats.losses++;
			}
			else
			{
				stats.draws++;
			}

			// Progress indicator
			if ((i + 1) % 10 == 0)
			{
				std::cout << "Completed " << (i + 1) << "/" << gameCount << " games..." << std::endl;
			}
		}

		// Calculate final statistics
		stats.calculateStats();

		// Calculate average duration and move count
		std::chrono::milliseconds totalDuration{0};
		int						  totalMoves = 0;
		for (const auto &game : games)
		{
			totalDuration += game.duration;
			totalMoves += game.moveCount;
		}

		if (stats.totalGames > 0)
		{
			stats.averageGameDuration = totalDuration / stats.totalGames;
			stats.averageMoveCount	  = totalMoves / stats.totalGames;
		}

		return stats;
	}


	void saveResultsToFile(const std::string &filename, const CPUPerformanceStats &stats, const std::string &testDescription)
	{
		std::ofstream file(filename, std::ios::app);
		if (file.is_open())
		{
			file << "=== " << testDescription << " ===" << std::endl;
			file << "Total Games: " << stats.totalGames << std::endl;
			file << "Wins: " << stats.wins << std::endl;
			file << "Losses: " << stats.losses << std::endl;
			file << "Draws: " << stats.draws << std::endl;
			file << "Win Rate: " << std::fixed << std::setprecision(2) << stats.winRate << "%" << std::endl;
			file << "Average Game Duration: " << stats.averageGameDuration.count() << "ms" << std::endl;
			file << "Average Move Count: " << stats.averageMoveCount << std::endl;
			file << std::endl;
		}
	}
};



} // namespace PerformanceTests