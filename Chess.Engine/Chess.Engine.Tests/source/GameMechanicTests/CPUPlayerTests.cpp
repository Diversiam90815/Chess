/*
  ==============================================================================
	Module:			CPUPlayer Tests
	Description:    Testing the CPUPlayer module from the chess engine
  ==============================================================================
*/

#include <gtest/gtest.h>
#include <thread>
#include <chrono>

#include "CPUPLayer/CPUPlayer.h"
#include "Generation/MoveGeneration.h"
#include "Validation/MoveValidation.h"
#include "Execution/MoveExecution.h"

namespace GameMechanicTests
{

class CPUPlayerTests : public ::testing::Test
{
protected:
	std::shared_ptr<ChessBoard>		mBoard;
	std::shared_ptr<MoveValidation> mValidation;
	std::shared_ptr<MoveExecution>	mExecution;
	std::shared_ptr<MoveGeneration> mGeneration;
	std::shared_ptr<MoveEvaluation> mEvaluation;
	std::shared_ptr<CPUPlayer>		mCPUPlayer;

	void							SetUp() override
	{
		mBoard = std::make_shared<ChessBoard>();
		mBoard->initializeBoard();
		mValidation = std::make_shared<MoveValidation>(mBoard);
		mExecution	= std::make_shared<MoveExecution>(mBoard, mValidation);
		mGeneration = std::make_shared<MoveGeneration>(mBoard, mValidation, mExecution);
		mEvaluation = std::make_shared<MoveEvaluation>(mBoard, mGeneration);
		mCPUPlayer	= std::make_shared<CPUPlayer>(mGeneration, mEvaluation, mBoard);
	}
};


TEST_F(CPUPlayerTests, ConstructorInitializeCorrectly)
{
	// Verify CPU player is constructed with correct default values
	CPUConfiguration config = mCPUPlayer->getCPUConfiguration();

	EXPECT_EQ(config.difficulty, CPUDifficulty::Random) << "Default difficulty should be Random";
	EXPECT_EQ(config.cpuColor, PlayerColor::Black) << "Default CPU color should be Black";
	EXPECT_FALSE(config.enabled) << "CPU should be disabled by default";
	EXPECT_EQ(config.thinkingTime.count(), 1000) << "Default thinking time should be 1000ms";
}


TEST_F(CPUPlayerTests, SetCPUConfigurationUpdatesCorrectly)
{
	CPUConfiguration config;
	config.cpuColor		= PlayerColor::White;
	config.difficulty	= CPUDifficulty::Hard;
	config.enabled		= true;
	config.thinkingTime = std::chrono::milliseconds(2000);

	mCPUPlayer->setCPUConfiguration(config);
	CPUConfiguration retrievedConfig = mCPUPlayer->getCPUConfiguration();

	EXPECT_EQ(retrievedConfig.difficulty, CPUDifficulty::Hard) << "Difficulty should be updated to Hard";
	EXPECT_EQ(retrievedConfig.cpuColor, PlayerColor::White) << "CPU color should be updated to White";
	EXPECT_TRUE(retrievedConfig.enabled) << "CPU should be enabled";
	EXPECT_EQ(retrievedConfig.thinkingTime.count(), 2000) << "Thinking time should be updated to 2000ms";
}


TEST_F(CPUPlayerTests, IsCPUPlayerReturnsFalseWhenDisabled)
{
	CPUConfiguration config;
	config.enabled	= false;
	config.cpuColor = PlayerColor::Black;

	mCPUPlayer->setCPUConfiguration(config);

	bool isBlackCPU = mCPUPlayer->isCPUPlayer(PlayerColor::Black);
	bool isWhiteCPU = mCPUPlayer->isCPUPlayer(PlayerColor::White);

	EXPECT_FALSE(isBlackCPU) << "Should return false when CPU is disabled";
	EXPECT_FALSE(isWhiteCPU) << "Should return false when CPU is disabled";
}


TEST_F(CPUPlayerTests, IsCPUPlayerReturnsTrueForCorrectColor)
{
	CPUConfiguration config;
	config.enabled	= true;
	config.cpuColor = PlayerColor::Black;

	mCPUPlayer->setCPUConfiguration(config);

	bool isBlackCPU = mCPUPlayer->isCPUPlayer(PlayerColor::Black);
	bool isWhiteCPU = mCPUPlayer->isCPUPlayer(PlayerColor::White);

	EXPECT_TRUE(isBlackCPU) << "Should return true for CPU color when enabled";
	EXPECT_FALSE(isWhiteCPU) << "Should return false for non-CPU color";
}


TEST_F(CPUPlayerTests, IsCPUEnabledReturnsCorrectState)
{
	// Test disabled state
	CPUConfiguration config;
	config.enabled = false;

	mCPUPlayer->setCPUConfiguration(config);

	EXPECT_FALSE(mCPUPlayer->isCPUEnabled()) << "Should return false when disabled";

	// Test enabled state
	config.enabled = true;
	mCPUPlayer->setCPUConfiguration(config);

	EXPECT_TRUE(mCPUPlayer->isCPUEnabled()) << "Should return true when enabled";
}


TEST_F(CPUPlayerTests, SetEnabledUpdatesCorrectly)
{
	mCPUPlayer->setEnabled(true);
	EXPECT_TRUE(mCPUPlayer->isCPUEnabled()) << "Should be enabled after setEnabled(true)";

	mCPUPlayer->setEnabled(false);
	EXPECT_FALSE(mCPUPlayer->isCPUEnabled()) << "Should be disabled after setEnabled(false)";
}


TEST_F(CPUPlayerTests, GetRandomMoveReturnsValidMove)
{
	std::vector<PossibleMove> moves;

	// Create test moves
	PossibleMove			  move1{{0, 1}, {0, 2}, MoveType::Normal};
	PossibleMove			  move2{{1, 1}, {1, 2}, MoveType::Normal};
	PossibleMove			  move3{{2, 1}, {2, 2}, MoveType::Normal};
	PossibleMove			  move4{{2, 2}, {2, 3}, MoveType::Normal};
	PossibleMove			  move5{{2, 3}, {2, 4}, MoveType::Normal};

	moves.push_back(move1);
	moves.push_back(move2);
	moves.push_back(move3);
	moves.push_back(move4);
	moves.push_back(move5);

	// Test multiple times to ensure randomness works
	bool		 foundDifferentMoves = false;
	PossibleMove firstMove			 = mCPUPlayer->getRandomMove(moves);

	for (int i = 0; i < 10; ++i)
	{
		PossibleMove randomMove	 = mCPUPlayer->getRandomMove(moves);

		// Verify the move is one of our test moves
		bool		 isValidMove = (randomMove == move1) || (randomMove == move2) || (randomMove == move3 || (randomMove == move4) || (randomMove == move5));

		EXPECT_TRUE(isValidMove) << "Random move should be one of the provided moves";

		// Check if we get different moves (for randomness)
		if (!(randomMove == firstMove) && !foundDifferentMoves)
		{
			foundDifferentMoves = true;
		}
	}

	// Note: This test might occasionally fail due to randomness, but it's unlikely
	EXPECT_TRUE(foundDifferentMoves) << "Random move selection should produce different moves over multiple calls";
}


TEST_F(CPUPlayerTests, GetEasyMovePrefersCheckmate)
{
	std::vector<PossibleMove> moves;

	// Create moves with different basic evaluation scores
	PossibleMove			  normalMove{{0, 1}, {0, 2}, MoveType::Normal};
	PossibleMove			  captureMove{{1, 1}, {1, 2}, MoveType::Capture};
	PossibleMove			  checkmateMove{{2, 1}, {2, 2}, MoveType::Checkmate};

	moves.push_back(normalMove);
	moves.push_back(captureMove);
	moves.push_back(checkmateMove);

	PossibleMove selectedMove = mCPUPlayer->getEasyMove(moves);

	// Checkmate should be selected as it has the highest basic evaluation score (1000)
	EXPECT_TRUE(selectedMove == checkmateMove) << "Easy mode should select the move with highest basic evaluation (checkmate)";
}


TEST_F(CPUPlayerTests, RequestMoveAsyncDoesNothingForNonCPUPlayer)
{
	CPUConfiguration config;
	config.enabled	= true;
	config.cpuColor = PlayerColor::Black;

	mCPUPlayer->setCPUConfiguration(config);

	// This should not cause any issues and should return quickly
	mCPUPlayer->requestMoveAsync(PlayerColor::White);

	// If we reach this point without hanging, the test passes
	SUCCEED() << "requestMoveAsync should handle non-CPU player gracefully";
}

} // namespace GameMechanicTests
