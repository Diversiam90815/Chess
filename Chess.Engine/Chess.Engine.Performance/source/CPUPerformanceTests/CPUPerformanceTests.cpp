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
	std::shared_ptr<ChessBoard>			  mBoard;
	std::shared_ptr<MoveValidation>		  mValidation;
	std::shared_ptr<MoveExecution>		  mExecution;
	std::shared_ptr<MoveGeneration>		  mGeneration;
	std::shared_ptr<MoveEvaluation>		  mEvaluation;

	std::shared_ptr<CPUPlayer>			  mWhiteCPU;
	std::shared_ptr<CPUPlayer>			  mBlackCPU;

	std::atomic<bool>					  mGameCompleted{false};
	std::atomic<bool>					  mGameRunning{false};
	GameResult							  mCurrentGameResult;
	std::chrono::steady_clock::time_point mGameStartTime;


	void								  SetUp() override
	{
		// Initialize core components
		mBoard = std::make_shared<ChessBoard>();
		mBoard->initializeBoard();

		mValidation = std::make_shared<MoveValidation>(mBoard);
		mExecution	= std::make_shared<MoveExecution>(mBoard, mValidation);
		mGeneration = std::make_shared<MoveGeneration>(mBoard, mValidation, mExecution);
		mEvaluation = std::make_shared<MoveEvaluation>(mBoard);

		// Create CPU players
		mWhiteCPU	= std::make_shared<CPUPlayer>(mGeneration, mEvaluation, mBoard);
		mBlackCPU	= std::make_shared<CPUPlayer>(mGeneration, mEvaluation, mBoard);
	}
};



} // namespace PerformanceTests