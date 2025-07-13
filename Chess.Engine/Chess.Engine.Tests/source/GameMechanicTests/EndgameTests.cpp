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
		PossibleMove mutableMove = move;
		mEngine->executeMove(mutableMove);
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

		// Helper to execute Scholar's Mate sequence
	void ExecuteScholarsMate()
	{
		std::vector<PossibleMove> moveSequence = {
			CreateMove({4, 6}, {4, 4}), // e2-e4 (White)
			CreateMove({4, 1}, {4, 3}), // e7-e5 (Black)
			CreateMove({5, 7}, {2, 4}), // f1-c4 (White: Bc4)
			CreateMove({1, 0}, {2, 2}), // b8-c6 (Black: Nc6)
			CreateMove({3, 7}, {7, 3}), // d1-h5 (White: Qh5)
			CreateMove({6, 0}, {5, 2}), // g8-f6 (Black: Nf6??)
			CreateMove({7, 3}, {5, 1})	// h5xf7# (White: Qxf7# checkmate)
		};

		for (const auto &move : moveSequence)
		{
			ExecuteMove(move);
		}
	}

	// Helper to execute Fool's Mate sequence (fastest checkmate)
	void ExecuteFoolsMate()
	{
		std::vector<PossibleMove> moveSequence = {
			CreateMove({5, 6}, {5, 5}), // f2-f3 (White)
			CreateMove({4, 1}, {4, 3}), // e7-e5 (Black)
			CreateMove({6, 6}, {6, 4}), // g2-g4 (White)
			CreateMove({3, 0}, {7, 4})	// d8-h4# (Black: Qh4# checkmate)
		};

		for (const auto &move : moveSequence)
		{
			ExecuteMove(move);
		}
	}
};


// ============================================================================
// BASIC TESTS
// ============================================================================

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



// ============================================================================
// GAME SEQUENCE TESTS
// ============================================================================

TEST_F(EndgameTests, ScholarsMateSequence)
{
	mEngine->startGame();

	// Scholar's mate sequence
	std::vector<std::pair<PossibleMove, std::string>> moveSequence = {
		{CreateMove({4, 6}, {4, 4}), "e2-e4"},						// White: e4
		{CreateMove({4, 1}, {4, 3}), "e7-e5"},						// Black: e5
		{CreateMove({5, 7}, {2, 4}), "f1-c4"},						// White: Bc4
		{CreateMove({1, 0}, {2, 2}), "b8-c6"},						// Black: Nc6
		{CreateMove({3, 7}, {7, 3}), "d1-h5"},						// White: Qh5
		{CreateMove({6, 0}, {5, 2}), "g8-f6"},						// Black: Nf6??
		{CreateMove({7, 3}, {5, 1}, MoveType::Checkmate), "h5xf7#"} // White: Qxf7# (checkmate)
	};

	EndGameState gameState = EndGameState::OnGoing;

	for (size_t i = 0; i < moveSequence.size(); ++i)
	{
		const auto &[move, notation] = moveSequence[i];

		// Execute the move
		ExecuteMove(move);

		// Check game state after each move
		gameState = mEngine->checkForEndGameConditions();

		if (i < moveSequence.size() - 1)
		{
			// All moves except the last should result in ongoing game
			EXPECT_EQ(gameState, EndGameState::OnGoing) << "Game should be ongoing after move: " << notation;
		}
		else
		{
			// Last move should result in checkmate
			EXPECT_EQ(gameState, EndGameState::Checkmate) << "Final move should result in checkmate: " << notation;

			// Check that White is the winner
			auto winner = mEngine->getWinner();
			ASSERT_TRUE(winner.has_value()) << "Winner should be determined after checkmate";
			EXPECT_EQ(winner.value(), PlayerColor::White) << "White should be the winner";
		}
	}
}


// ============================================================================
// INTEGRATION TESTS
// ============================================================================

TEST_F(EndgameTests, MultipleGameSequences)
{
	// Test that we can play multiple complete games with checkmates using the same engine

	// Game 1: Scholar's Mate (White wins)
	{
		mEngine->startGame();
		ExecuteScholarsMate();

		EndGameState state = mEngine->checkForEndGameConditions();
		EXPECT_EQ(state, EndGameState::Checkmate) << "Game 1 should end in checkmate";

		auto winner = mEngine->getWinner();
		ASSERT_TRUE(winner.has_value()) << "Game 1 should have a winner";
		EXPECT_EQ(winner.value(), PlayerColor::White) << "White should win Game 1";

		// Reset for next game
		mEngine->resetGame();
	}

	// Game 2: Fool's Mate (Black wins)
	{
		mEngine->startGame();
		ExecuteFoolsMate();

		EndGameState state = mEngine->checkForEndGameConditions();
		EXPECT_EQ(state, EndGameState::Checkmate) << "Game 2 should end in checkmate";

		auto winner = mEngine->getWinner();
		ASSERT_TRUE(winner.has_value()) << "Game 2 should have a winner";
		EXPECT_EQ(winner.value(), PlayerColor::Black) << "Black should win Game 2";

		// Reset for next game
		mEngine->resetGame();
	}

	// Game 3: Normal game sequence (no checkmate)
	{
		mEngine->startGame();

		// Play some normal moves without reaching checkmate
		ExecuteMove(CreateMove({4, 6}, {4, 4})); // e2-e4
		ExecuteMove(CreateMove({4, 1}, {4, 3})); // e7-e5
		ExecuteMove(CreateMove({6, 7}, {5, 5})); // g1-f3
		ExecuteMove(CreateMove({1, 0}, {2, 2})); // b8-c6

		EndGameState state = mEngine->checkForEndGameConditions();
		EXPECT_EQ(state, EndGameState::OnGoing) << "Game 3 should be ongoing";

		auto winner = mEngine->getWinner();
		EXPECT_FALSE(winner.has_value()) << "Game 3 should have no winner yet";

		mEngine->resetGame();
	}

	// Verify final reset worked
	auto finalWinner = mEngine->getWinner();
	EXPECT_FALSE(finalWinner.has_value()) << "Winner should be cleared after final reset";
}





} // namespace GameMechanicTests