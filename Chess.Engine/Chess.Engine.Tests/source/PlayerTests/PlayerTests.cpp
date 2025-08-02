/*
  ==============================================================================
	Module:			Player Tests
	Description:    Testing the Player class functionality
  ==============================================================================
*/

#include <gtest/gtest.h>

#include "Player/Player.h"


namespace PlayerTests
{


class PlayerTests : public ::testing::Test
{
protected:
	void SetUp() override
	{
		mWhitePlayer = std::make_unique<Player>(PlayerColor::White);
		mBlackPlayer = std::make_unique<Player>(PlayerColor::Black);
	}

	std::unique_ptr<Player> mWhitePlayer;
	std::unique_ptr<Player> mBlackPlayer;
};


TEST_F(PlayerTests, DefaultConstructor)
{
	Player player;

	EXPECT_EQ(player.getPlayerColor(), PlayerColor::NoColor) << "Default player should have no color";
	EXPECT_EQ(player.getScore().getValue(), 0) << "Default player should have score of 0";
	EXPECT_TRUE(player.isLocalPlayer()) << "Default player should be local player";
}


TEST_F(PlayerTests, ParameterizedConstructor)
{
	EXPECT_EQ(mWhitePlayer->getPlayerColor(), PlayerColor::White) << "White player should have white color";
	EXPECT_EQ(mBlackPlayer->getPlayerColor(), PlayerColor::Black) << "Black player should have black color";
	EXPECT_EQ(mWhitePlayer->getScore().getValue(), 0) << "New white player should have score of 0";
	EXPECT_EQ(mBlackPlayer->getScore().getValue(), 0) << "New black player should have score of 0";
	EXPECT_TRUE(mWhitePlayer->isLocalPlayer()) << "New player should be local by default";
}



// =============================================================================
// SCORE MANAGEMENT TESTS
// =============================================================================


TEST_F(PlayerTests, ScoreInitialization)
{
	Score whiteScore = mWhitePlayer->getScore();
	Score blackScore = mBlackPlayer->getScore();

	EXPECT_EQ(whiteScore.getPlayerColor(), PlayerColor::White) << "White player score should have white color";
	EXPECT_EQ(blackScore.getPlayerColor(), PlayerColor::Black) << "Black player score should have black color";
	EXPECT_EQ(whiteScore.getValue(), 0) << "Initial white score should be 0";
	EXPECT_EQ(blackScore.getValue(), 0) << "Initial black score should be 0";
}


TEST_F(PlayerTests, SetScore)
{
	mWhitePlayer->setScore(150);
	mBlackPlayer->setScore(200);

	EXPECT_EQ(mWhitePlayer->getScore().getValue(), 150) << "White player score should be set to 150";
	EXPECT_EQ(mBlackPlayer->getScore().getValue(), 200) << "Black player score should be set to 200";
}


TEST_F(PlayerTests, ScoreAfterCaptures)
{
	// Add captured pieces and verify score updates
	mWhitePlayer->addCapturedPiece(PieceType::Pawn);   // +100
	mWhitePlayer->addCapturedPiece(PieceType::Knight); // +320
	mWhitePlayer->addCapturedPiece(PieceType::Queen);  // +900

	// Score should automatically update when pieces are captured
	int expectedScore = mWhitePlayer->getPieceValue(PieceType::Pawn) + mWhitePlayer->getPieceValue(PieceType::Knight) + mWhitePlayer->getPieceValue(PieceType::Queen);

	EXPECT_EQ(mWhitePlayer->getScore().getValue(), expectedScore) << "Score should reflect captured pieces value";
}



// =============================================================================
// PLAYER COLOR TESTS
// =============================================================================


TEST_F(PlayerTests, GetPlayerColor)
{
	EXPECT_EQ(mWhitePlayer->getPlayerColor(), PlayerColor::White) << "White player should return White color";
	EXPECT_EQ(mBlackPlayer->getPlayerColor(), PlayerColor::Black) << "Black player should return Black color";
}


TEST_F(PlayerTests, SetPlayerColor)
{
	Player player;
	player.setPlayerColor(PlayerColor::White);

	EXPECT_EQ(player.getPlayerColor(), PlayerColor::White) << "Player color should be set to White";

	player.setPlayerColor(PlayerColor::Black);
	EXPECT_EQ(player.getPlayerColor(), PlayerColor::Black) << "Player color should be changed to Black";
}



// =============================================================================
// CAPTURED PIECES TESTS
// =============================================================================


TEST_F(PlayerTests, AddCapturedPiece)
{
	mWhitePlayer->addCapturedPiece(PieceType::Pawn);
	mWhitePlayer->addCapturedPiece(PieceType::Knight);
	mWhitePlayer->addCapturedPiece(PieceType::Pawn); // Add another pawn

	// Verify score reflects captured pieces
	int expectedScore = 2 * mWhitePlayer->getPieceValue(PieceType::Pawn) + mWhitePlayer->getPieceValue(PieceType::Knight);

	EXPECT_EQ(mWhitePlayer->getScore().getValue(), expectedScore) << "Score should reflect all captured pieces";
}


TEST_F(PlayerTests, RemoveLastCapturedPiece)
{
	// Add some pieces
	mWhitePlayer->addCapturedPiece(PieceType::Pawn);
	mWhitePlayer->addCapturedPiece(PieceType::Knight);
	mWhitePlayer->addCapturedPiece(PieceType::Rook);

	int scoreAfterCaptures = mWhitePlayer->getScore().getValue();

	// Remove last captured piece (rook)
	mWhitePlayer->removeLastCapturedPiece();

	int expectedScore = mWhitePlayer->getPieceValue(PieceType::Pawn) + mWhitePlayer->getPieceValue(PieceType::Knight);

	EXPECT_EQ(mWhitePlayer->getScore().getValue(), expectedScore) << "Score should be reduced by last captured piece value";
	EXPECT_LT(mWhitePlayer->getScore().getValue(), scoreAfterCaptures) << "Score should be lower after removing piece";
}


TEST_F(PlayerTests, RemoveLastCapturedPieceFromEmpty)
{
	// Try to remove from empty captured pieces list
	int initialScore = mWhitePlayer->getScore().getValue();

	EXPECT_NO_THROW(mWhitePlayer->removeLastCapturedPiece()) << "Removing from empty list should not throw";
	EXPECT_EQ(mWhitePlayer->getScore().getValue(), initialScore) << "Score should remain unchanged when removing from empty list";
}



// =============================================================================
// SCORE UPDATE TESTS
// =============================================================================


TEST_F(PlayerTests, UpdateScore)
{
	// Add some captured pieces
	mWhitePlayer->addCapturedPiece(PieceType::Pawn);
	mWhitePlayer->addCapturedPiece(PieceType::Rook);

	int scoreBeforeUpdate = mWhitePlayer->getScore().getValue();

	// Call updateScore explicitly
	mWhitePlayer->updateScore();

	// Score should remain the same since it should already be up to date
	EXPECT_EQ(mWhitePlayer->getScore().getValue(), scoreBeforeUpdate) << "Manual score update should maintain consistency";
}


TEST_F(PlayerTests, ScoreUpdateAfterMultipleCaptures)
{
	// Simulate capturing all piece types
	mWhitePlayer->addCapturedPiece(PieceType::Pawn);
	mWhitePlayer->addCapturedPiece(PieceType::Pawn);
	mWhitePlayer->addCapturedPiece(PieceType::Knight);
	mWhitePlayer->addCapturedPiece(PieceType::Bishop);
	mWhitePlayer->addCapturedPiece(PieceType::Rook);
	mWhitePlayer->addCapturedPiece(PieceType::Queen);

	int expectedScore = 2 * mWhitePlayer->getPieceValue(PieceType::Pawn) + mWhitePlayer->getPieceValue(PieceType::Knight) + mWhitePlayer->getPieceValue(PieceType::Bishop) +
						mWhitePlayer->getPieceValue(PieceType::Rook) + mWhitePlayer->getPieceValue(PieceType::Queen);

	EXPECT_EQ(mWhitePlayer->getScore().getValue(), expectedScore) << "Score should correctly sum all captured pieces";
}



// =============================================================================
// RESET FUNCTIONALITY TESTS
// =============================================================================


TEST_F(PlayerTests, Reset)
{
	// Set up player with some captured pieces and score
	mWhitePlayer->addCapturedPiece(PieceType::Queen);
	mWhitePlayer->addCapturedPiece(PieceType::Rook);
	mWhitePlayer->setScore(1500);

	EXPECT_GT(mWhitePlayer->getScore().getValue(), 0) << "Player should have non-zero score before reset";

	// Reset the player
	mWhitePlayer->reset();

	EXPECT_EQ(mWhitePlayer->getScore().getValue(), 0) << "Score should be 0 after reset";
	// Note: We can't directly test if captured pieces are cleared without access to the vector,
	// but the score being 0 indicates they were cleared
}


TEST_F(PlayerTests, ResetPreservesPlayerColor)
{
	PlayerColor originalColor = mWhitePlayer->getPlayerColor();

	mWhitePlayer->addCapturedPiece(PieceType::Queen);
	mWhitePlayer->reset();

	EXPECT_EQ(mWhitePlayer->getPlayerColor(), originalColor) << "Player color should be preserved after reset";
}



// =============================================================================
// LOCAL PLAYER TESTS
// =============================================================================


TEST_F(PlayerTests, IsLocalPlayerDefault)
{
	EXPECT_TRUE(mWhitePlayer->isLocalPlayer()) << "Player should be local by default";
	EXPECT_TRUE(mBlackPlayer->isLocalPlayer()) << "Player should be local by default";
}


TEST_F(PlayerTests, SetIsLocalPlayer)
{
	mWhitePlayer->setIsLocalPlayer(false);
	EXPECT_FALSE(mWhitePlayer->isLocalPlayer()) << "Player should not be local after setting to false";

	mWhitePlayer->setIsLocalPlayer(true);
	EXPECT_TRUE(mWhitePlayer->isLocalPlayer()) << "Player should be local after setting to true";
}






} // namespace PlayerTests

