/*
  ==============================================================================
	Module:			Endgame Tests
	Description:    Testing the endgame conditions and winner determination
  ==============================================================================
*/


#include <gtest/gtest.h>

#include "GameEngine.h"


namespace GameMechanicTests
{

class EndgameTests : public ::testing::Test
{
protected:
	std::unique_ptr<GameEngine> mEngine;


	void						SetUp() override
	{
		mEngine = std::make_unique<GameEngine>();
		mEngine->init();
	}


	void TearDown() override
	{
		mEngine->resetGame();
		mEngine.reset();
	}


	// Helper to execute moves through GameEngine
	void ExecuteMove(const PossibleMove &move)
	{
		mEngine->calculateAllMovesForPlayer();
		mEngine->initiateMove(move.start);

		PossibleMove mutableMove = move;
		mEngine->executeMove(mutableMove);

		mEngine->switchTurns();
	}

	// Helper to create a basic move
	PossibleMove CreateMove(Position start, Position end, MoveType type = MoveType::Normal)
	{
		PossibleMove move;
		move.start			= start;
		move.end			= end;
		move.type			= type;
		move.promotionPiece = PieceType::DefaultType;
		return move;
	}
};


TEST_F(EndgameTests, GetWinnerReturnsNulloptWhenNoMoves)
{
	auto winner = mEngine->getWinner();

	EXPECT_FALSE(winner.has_value()) << "getWinner should return nullopt when no moves have been made";
}


TEST_F(EndgameTests, GetWinnerAfterNormalMove)
{
	// Start the game to initialize the board
	mEngine->startGame();

	// Execute a normal pawn move (e2-e4)
	PossibleMove normalMove = CreateMove({4, 6}, {4, 4}, MoveType::Normal);
	ExecuteMove(normalMove);

	auto winner = mEngine->getWinner();
	EXPECT_FALSE(winner.has_value()) << "getWinner should return nullopt for non-checkmate moves";
}


TEST_F(EndgameTests, GetWinnerReturnsWhiteForScholarsMate)
{
	// Execute Scholar's Mate where White wins
	mEngine->startGame();

	ExecuteMove(CreateMove({4, 6}, {4, 4}, MoveType::Normal));	// 1. e4
	ExecuteMove(CreateMove({4, 1}, {4, 3}, MoveType::Normal));	// 1... e5
	ExecuteMove(CreateMove({5, 7}, {2, 4}, MoveType::Normal));	// 2. Bc4
	ExecuteMove(CreateMove({1, 0}, {2, 2}, MoveType::Normal));	// 2... Nc6
	ExecuteMove(CreateMove({3, 7}, {7, 3}, MoveType::Normal));	// 3. Qh5
	ExecuteMove(CreateMove({6, 0}, {5, 2}, MoveType::Normal));	// 3... Nf6??
	ExecuteMove(CreateMove({7, 3}, {5, 1}, MoveType::Capture)); // 4. Qxf7# (checkmate by White)

	// Verify White is the winner
	auto winner = mEngine->getWinner();
	ASSERT_TRUE(winner.has_value()) << "Winner should be determined after checkmate";
	EXPECT_EQ(winner.value(), PlayerColor::White) << "White should be the winner after delivering checkmate";
}


TEST_F(EndgameTests, GetWinnerReturnsBlackForFoolsMate)
{
	mEngine->startGame();

	ExecuteMove(CreateMove({5, 6}, {5, 5}, MoveType::Normal)); // 1. f3
	ExecuteMove(CreateMove({4, 1}, {4, 3}, MoveType::Normal)); // 1... e5
	ExecuteMove(CreateMove({6, 6}, {6, 4}, MoveType::Normal)); // 2. g4??
	ExecuteMove(CreateMove({3, 0}, {7, 4}, MoveType::Normal)); // 2... Qh4# (checkmate by Black)

	// Verify Black is the winner
	auto winner = mEngine->getWinner();
	ASSERT_TRUE(winner.has_value()) << "Winner should be determined after checkmate";
	EXPECT_EQ(winner.value(), PlayerColor::Black) << "Black should be the winner after delivering checkmate";
}


TEST_F(EndgameTests, CheckForEndgameConditionsHandlingNoLastMove)
{
	// Test when there's no last move in history
	EndGameState state = mEngine->checkForEndGameConditions();
	EXPECT_EQ(state, EndGameState::OnGoing) << "checkForEndGameConditions should return OnGoing when no moves have been made";
}


TEST_F(EndgameTests, CheckForEndGameConditionsReturnsOngoingForNormalGame)
{
	// Start with initial board position
	mEngine->startGame();

	// Make a normal opening move (e2-e4)
	PossibleMove normalMove = CreateMove({4, 6}, {4, 4}, MoveType::Normal);
	ExecuteMove(normalMove);

	EndGameState result = mEngine->checkForEndGameConditions();
	EXPECT_EQ(result, EndGameState::OnGoing) << "checkForEndGameConditions should return OnGoing for normal game";
}


TEST_F(EndgameTests, DetectsCheckmateAndReturnsCorrectWinner)
{
	// Execute a series of moves leading to Scholar's Mate
	mEngine->startGame();

	ExecuteMove(CreateMove({4, 6}, {4, 4}, MoveType::Normal));	// 1. e4
	ExecuteMove(CreateMove({4, 1}, {4, 3}, MoveType::Normal));	// 1... e5
	ExecuteMove(CreateMove({5, 7}, {2, 4}, MoveType::Normal));	// 2. Bc4
	ExecuteMove(CreateMove({1, 0}, {2, 2}, MoveType::Normal));	// 2... Nc6
	ExecuteMove(CreateMove({3, 7}, {7, 3}, MoveType::Normal));	// 3. Qh5
	ExecuteMove(CreateMove({6, 0}, {5, 2}, MoveType::Normal));	// 3... Nf6??
	ExecuteMove(CreateMove({7, 3}, {5, 1}, MoveType::Capture)); // 4. Qxf7# (checkmate)

	// Check that the game detects checkmate
	EndGameState result = mEngine->checkForEndGameConditions();
	EXPECT_EQ(result, EndGameState::Checkmate) << "Should detect checkmate";

	// Check that the winner is correctly identified as White
	auto winner = mEngine->getWinner();
	ASSERT_TRUE(winner.has_value()) << "Winner should be determined in checkmate";
	EXPECT_EQ(winner.value(), PlayerColor::White) << "White should be the winner";
}


} // namespace GameMechanicTests