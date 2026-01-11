/*
  ==============================================================================
	Module:			Player Tests
	Description:    Testing the Player class functionality
  ==============================================================================
*/

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Player/Player.h"
#include "IObserver.h"


namespace PlayerTests
{

class MockPlayerObserver : public IPlayerObserver
{
public:
	MOCK_METHOD(void, onAddCapturedPiece, (Side player, PieceType piece), (override));
	MOCK_METHOD(void, onRemoveLastCapturedPiece, (Side player, PieceType piece), (override));
};


class PlayerTests : public ::testing::Test
{
protected:
	void SetUp() override
	{
		mWhitePlayer  = std::make_unique<Player>(Side::White);
		mBlackPlayer  = std::make_unique<Player>(Side::Black);
		mMockObserver = std::make_shared<MockPlayerObserver>();
	}

	std::unique_ptr<Player>				mWhitePlayer;
	std::unique_ptr<Player>				mBlackPlayer;
	std::shared_ptr<MockPlayerObserver> mMockObserver;
};


//=============================================================================
// CONSTRUCTOR TESTS
//=============================================================================

TEST_F(PlayerTests, DefaultConstructor)
{
	Player player;

	EXPECT_EQ(player.getPlayerColor(), Side::None) << "Default player should have no color";
	EXPECT_TRUE(player.isLocalPlayer()) << "Default player should be local player";
}


TEST_F(PlayerTests, ParameterizedConstructor)
{
	EXPECT_EQ(mWhitePlayer->getPlayerColor(), Side::White) << "White player should have white color";
	EXPECT_EQ(mBlackPlayer->getPlayerColor(), Side::Black) << "Black player should have black color";
	EXPECT_TRUE(mWhitePlayer->isLocalPlayer()) << "New player should be local by default";
	EXPECT_TRUE(mBlackPlayer->isLocalPlayer()) << "New player should be local by default";
}


//=============================================================================
// PLAYER COLOR TESTS
//=============================================================================

TEST_F(PlayerTests, GetPlayerColor)
{
	EXPECT_EQ(mWhitePlayer->getPlayerColor(), Side::White) << "White player should return White color";
	EXPECT_EQ(mBlackPlayer->getPlayerColor(), Side::Black) << "Black player should return Black color";
}


TEST_F(PlayerTests, SetPlayerColor)
{
	Player player;
	player.setPlayerColor(Side::White);

	EXPECT_EQ(player.getPlayerColor(), Side::White) << "Player color should be set to White";

	player.setPlayerColor(Side::Black);
	EXPECT_EQ(player.getPlayerColor(), Side::Black) << "Player color should be changed to Black";
}


TEST_F(PlayerTests, SetPlayerColorToSameValue)
{
	mWhitePlayer->setPlayerColor(Side::White);
	EXPECT_EQ(mWhitePlayer->getPlayerColor(), Side::White) << "Setting same color should work";
}


TEST_F(PlayerTests, SetPlayerColorToNone)
{
	mWhitePlayer->setPlayerColor(Side::None);
	EXPECT_EQ(mWhitePlayer->getPlayerColor(), Side::None) << "Player color can be set to None";
}


//=============================================================================
// CAPTURED PIECES TESTS
//=============================================================================

TEST_F(PlayerTests, AddCapturedPiece)
{
	mWhitePlayer->attachObserver(mMockObserver);

	EXPECT_CALL(*mMockObserver, onAddCapturedPiece(Side::White, PieceType::BPawn)).Times(1);

	mWhitePlayer->addCapturedPiece(PieceType::BPawn);
}


TEST_F(PlayerTests, AddMultipleCapturedPieces)
{
	mWhitePlayer->attachObserver(mMockObserver);

	EXPECT_CALL(*mMockObserver, onAddCapturedPiece(Side::White, PieceType::BPawn)).Times(1);
	EXPECT_CALL(*mMockObserver, onAddCapturedPiece(Side::White, PieceType::BKnight)).Times(1);
	EXPECT_CALL(*mMockObserver, onAddCapturedPiece(Side::White, PieceType::BRook)).Times(1);

	mWhitePlayer->addCapturedPiece(PieceType::BPawn);
	mWhitePlayer->addCapturedPiece(PieceType::BKnight);
	mWhitePlayer->addCapturedPiece(PieceType::BRook);
}


TEST_F(PlayerTests, AddCapturedPieceSameTypeMultipleTimes)
{
	mWhitePlayer->attachObserver(mMockObserver);

	// Should notify each time, even for the same piece type
	EXPECT_CALL(*mMockObserver, onAddCapturedPiece(Side::White, PieceType::BPawn)).Times(3);

	mWhitePlayer->addCapturedPiece(PieceType::BPawn);
	mWhitePlayer->addCapturedPiece(PieceType::BPawn);
	mWhitePlayer->addCapturedPiece(PieceType::BPawn);
}


TEST_F(PlayerTests, AddCapturedPieceNotifiesAllObservers)
{
	auto observer2 = std::make_shared<MockPlayerObserver>();

	mWhitePlayer->attachObserver(mMockObserver);
	mWhitePlayer->attachObserver(observer2);

	// Both observers should be notified
	EXPECT_CALL(*mMockObserver, onAddCapturedPiece(Side::White, PieceType::BQueen)).Times(1);
	EXPECT_CALL(*observer2, onAddCapturedPiece(Side::White, PieceType::BQueen)).Times(1);

	mWhitePlayer->addCapturedPiece(PieceType::BQueen);
}


TEST_F(PlayerTests, RemoveLastCapturedPiece)
{
	mWhitePlayer->attachObserver(mMockObserver);

	// Add pieces
	mWhitePlayer->addCapturedPiece(PieceType::BPawn);
	mWhitePlayer->addCapturedPiece(PieceType::BKnight);
	mWhitePlayer->addCapturedPiece(PieceType::BRook);

	// Remove last piece (rook)
	EXPECT_CALL(*mMockObserver, onRemoveLastCapturedPiece(Side::White, PieceType::BRook)).Times(1);

	mWhitePlayer->removeLastCapturedPiece();
}


TEST_F(PlayerTests, RemoveLastCapturedPieceFromEmpty)
{
	// Try to remove from empty captured pieces list
	// Should log warning but not crash or notify observers

	EXPECT_CALL(*mMockObserver, onRemoveLastCapturedPiece(::testing::_, ::testing::_)).Times(0);

	mWhitePlayer->attachObserver(mMockObserver);
	EXPECT_NO_THROW(mWhitePlayer->removeLastCapturedPiece()) << "Removing from empty list should not throw";
}


TEST_F(PlayerTests, RemoveLastCapturedPieceSequence)
{
	mWhitePlayer->attachObserver(mMockObserver);

	// Add pieces
	mWhitePlayer->addCapturedPiece(PieceType::BPawn);
	mWhitePlayer->addCapturedPiece(PieceType::BKnight);
	mWhitePlayer->addCapturedPiece(PieceType::BRook);

	// Remove in reverse order
	EXPECT_CALL(*mMockObserver, onRemoveLastCapturedPiece(Side::White, PieceType::BRook)).Times(1);
	mWhitePlayer->removeLastCapturedPiece();

	EXPECT_CALL(*mMockObserver, onRemoveLastCapturedPiece(Side::White, PieceType::BKnight)).Times(1);
	mWhitePlayer->removeLastCapturedPiece();

	EXPECT_CALL(*mMockObserver, onRemoveLastCapturedPiece(Side::White, PieceType::BPawn)).Times(1);
	mWhitePlayer->removeLastCapturedPiece();
}


TEST_F(PlayerTests, RemoveLastCapturedPieceNotifiesAllObservers)
{
	auto observer2 = std::make_shared<MockPlayerObserver>();

	mWhitePlayer->attachObserver(mMockObserver);
	mWhitePlayer->attachObserver(observer2);

	mWhitePlayer->addCapturedPiece(PieceType::BQueen);

	// Both observers should be notified
	EXPECT_CALL(*mMockObserver, onRemoveLastCapturedPiece(Side::White, PieceType::BQueen)).Times(1);
	EXPECT_CALL(*observer2, onRemoveLastCapturedPiece(Side::White, PieceType::BQueen)).Times(1);

	mWhitePlayer->removeLastCapturedPiece();
}


//=============================================================================
// RESET FUNCTIONALITY TESTS
//=============================================================================

TEST_F(PlayerTests, Reset)
{
	// Add some captured pieces
	mWhitePlayer->addCapturedPiece(PieceType::BQueen);
	mWhitePlayer->addCapturedPiece(PieceType::BRook);
	mWhitePlayer->addCapturedPiece(PieceType::BKnight);

	// Reset the player
	mWhitePlayer->reset();

	// Try to remove a piece - should fail since list is empty
	mWhitePlayer->attachObserver(mMockObserver);
	EXPECT_CALL(*mMockObserver, onRemoveLastCapturedPiece(::testing::_, ::testing::_)).Times(0);

	EXPECT_NO_THROW(mWhitePlayer->removeLastCapturedPiece()) << "Should not crash when removing from empty list after reset";
}


TEST_F(PlayerTests, ResetPreservesPlayerColor)
{
	Side originalColor = mWhitePlayer->getPlayerColor();

	mWhitePlayer->addCapturedPiece(PieceType::BQueen);
	mWhitePlayer->reset();

	EXPECT_EQ(mWhitePlayer->getPlayerColor(), originalColor) << "Player color should be preserved after reset";
}


TEST_F(PlayerTests, ResetPreservesLocalPlayerStatus)
{
	mWhitePlayer->setIsLocalPlayer(false);
	bool originalLocalStatus = mWhitePlayer->isLocalPlayer();

	mWhitePlayer->addCapturedPiece(PieceType::BQueen);
	mWhitePlayer->reset();

	EXPECT_EQ(mWhitePlayer->isLocalPlayer(), originalLocalStatus) << "Local player status should be preserved after reset";
}


TEST_F(PlayerTests, ResetAfterMultipleOperations)
{
	// Perform multiple operations
	mWhitePlayer->setPlayerColor(Side::Black);
	mWhitePlayer->setIsLocalPlayer(false);
	mWhitePlayer->addCapturedPiece(PieceType::BQueen);
	mWhitePlayer->addCapturedPiece(PieceType::BRook);
	mWhitePlayer->addCapturedPiece(PieceType::BKnight);

	// Reset
	mWhitePlayer->reset();

	// Verify state
	EXPECT_EQ(mWhitePlayer->getPlayerColor(), Side::Black) << "Color should be preserved";
	EXPECT_FALSE(mWhitePlayer->isLocalPlayer()) << "Local status should be preserved";

	// Verify captured pieces are cleared
	mWhitePlayer->attachObserver(mMockObserver);
	EXPECT_CALL(*mMockObserver, onRemoveLastCapturedPiece(::testing::_, ::testing::_)).Times(0);
	EXPECT_NO_THROW(mWhitePlayer->removeLastCapturedPiece());
}


TEST_F(PlayerTests, ResetMultipleTimes)
{
	// Reset multiple times should work
	mWhitePlayer->addCapturedPiece(PieceType::BPawn);
	mWhitePlayer->reset();

	mWhitePlayer->addCapturedPiece(PieceType::BKnight);
	mWhitePlayer->reset();

	mWhitePlayer->addCapturedPiece(PieceType::BRook);
	mWhitePlayer->reset();

	// Should still work after multiple resets
	mWhitePlayer->attachObserver(mMockObserver);
	EXPECT_CALL(*mMockObserver, onAddCapturedPiece(Side::White, PieceType::BQueen)).Times(1);
	mWhitePlayer->addCapturedPiece(PieceType::BQueen);
}


//=============================================================================
// LOCAL PLAYER TESTS
//=============================================================================

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


TEST_F(PlayerTests, SetIsLocalPlayerMultipleTimes)
{
	// Toggle multiple times
	mWhitePlayer->setIsLocalPlayer(false);
	EXPECT_FALSE(mWhitePlayer->isLocalPlayer()) << "Should be remote";

	mWhitePlayer->setIsLocalPlayer(false); // Set to same value
	EXPECT_FALSE(mWhitePlayer->isLocalPlayer()) << "Should remain remote";

	mWhitePlayer->setIsLocalPlayer(true);
	EXPECT_TRUE(mWhitePlayer->isLocalPlayer()) << "Should be local again";

	mWhitePlayer->setIsLocalPlayer(true); // Set to same value
	EXPECT_TRUE(mWhitePlayer->isLocalPlayer()) << "Should remain local";
}


TEST_F(PlayerTests, LocalPlayerStatusIndependentOfOtherProperties)
{
	// Test that local player status is independent of other properties
	mWhitePlayer->setIsLocalPlayer(false);
	mWhitePlayer->addCapturedPiece(PieceType::BQueen);
	mWhitePlayer->setPlayerColor(Side::Black);

	EXPECT_FALSE(mWhitePlayer->isLocalPlayer()) << "Local status should be independent of other operations";
}


TEST_F(PlayerTests, LocalPlayerStatusForBothPlayers)
{
	// Test that both players can have different local status
	mWhitePlayer->setIsLocalPlayer(true);
	mBlackPlayer->setIsLocalPlayer(false);

	EXPECT_TRUE(mWhitePlayer->isLocalPlayer()) << "White should be local";
	EXPECT_FALSE(mBlackPlayer->isLocalPlayer()) << "Black should be remote";
}


//=============================================================================
// OBSERVER PATTERN TESTS
//=============================================================================

TEST_F(PlayerTests, AttachObserver)
{
	mWhitePlayer->attachObserver(mMockObserver);

	EXPECT_CALL(*mMockObserver, onAddCapturedPiece(Side::White, PieceType::BPawn)).Times(1);

	mWhitePlayer->addCapturedPiece(PieceType::BPawn);
}


TEST_F(PlayerTests, DetachObserver)
{
	mWhitePlayer->attachObserver(mMockObserver);
	mWhitePlayer->addCapturedPiece(PieceType::BPawn); // Should notify

	mWhitePlayer->detachObserver(mMockObserver);

	// Should not notify after detach
	EXPECT_CALL(*mMockObserver, onAddCapturedPiece(::testing::_, ::testing::_)).Times(0);

	mWhitePlayer->addCapturedPiece(PieceType::BKnight);
}


TEST_F(PlayerTests, MultipleObservers)
{
	auto observer2 = std::make_shared<MockPlayerObserver>();
	auto observer3 = std::make_shared<MockPlayerObserver>();

	mWhitePlayer->attachObserver(mMockObserver);
	mWhitePlayer->attachObserver(observer2);
	mWhitePlayer->attachObserver(observer3);

	// All should be notified
	EXPECT_CALL(*mMockObserver, onAddCapturedPiece(Side::White, PieceType::BQueen)).Times(1);
	EXPECT_CALL(*observer2, onAddCapturedPiece(Side::White, PieceType::BQueen)).Times(1);
	EXPECT_CALL(*observer3, onAddCapturedPiece(Side::White, PieceType::BQueen)).Times(1);

	mWhitePlayer->addCapturedPiece(PieceType::BQueen);
}


TEST_F(PlayerTests, WeakPtrObserverCleanup)
{
	{
		auto tempObserver = std::make_shared<MockPlayerObserver>();
		mWhitePlayer->attachObserver(tempObserver);

		EXPECT_CALL(*tempObserver, onAddCapturedPiece(Side::White, PieceType::BPawn)).Times(1);
		mWhitePlayer->addCapturedPiece(PieceType::BPawn);
	} // tempObserver goes out of scope

	// Should not crash even though observer was destroyed
	EXPECT_NO_THROW(mWhitePlayer->addCapturedPiece(PieceType::BKnight));
}


//=============================================================================
// EDGE CASES
//=============================================================================

TEST_F(PlayerTests, AddCapturedPieceNone)
{
	mWhitePlayer->attachObserver(mMockObserver);

	EXPECT_CALL(*mMockObserver, onAddCapturedPiece(Side::White, PieceType::None)).Times(1);

	mWhitePlayer->addCapturedPiece(PieceType::None);
}


TEST_F(PlayerTests, LargeNumberOfCapturedPieces)
{
	// Test with many captured pieces
	mWhitePlayer->attachObserver(mMockObserver);

	EXPECT_CALL(*mMockObserver, onAddCapturedPiece(Side::White, PieceType::BPawn)).Times(100);

	for (int i = 0; i < 100; ++i)
	{
		mWhitePlayer->addCapturedPiece(PieceType::BPawn);
	}
}


TEST_F(PlayerTests, AddAndRemoveSequence)
{
	mWhitePlayer->attachObserver(mMockObserver);

	// Add and remove sequence
	EXPECT_CALL(*mMockObserver, onAddCapturedPiece(Side::White, PieceType::BPawn)).Times(1);
	mWhitePlayer->addCapturedPiece(PieceType::BPawn);

	EXPECT_CALL(*mMockObserver, onRemoveLastCapturedPiece(Side::White, PieceType::BPawn)).Times(1);
	mWhitePlayer->removeLastCapturedPiece();

	EXPECT_CALL(*mMockObserver, onAddCapturedPiece(Side::White, PieceType::BKnight)).Times(1);
	mWhitePlayer->addCapturedPiece(PieceType::BKnight);

	EXPECT_CALL(*mMockObserver, onRemoveLastCapturedPiece(Side::White, PieceType::BKnight)).Times(1);
	mWhitePlayer->removeLastCapturedPiece();
}


TEST_F(PlayerTests, BlackPlayerOperations)
{
	// Test that black player works the same way
	mBlackPlayer->attachObserver(mMockObserver);

	EXPECT_CALL(*mMockObserver, onAddCapturedPiece(Side::Black, PieceType::WPawn)).Times(1);
	mBlackPlayer->addCapturedPiece(PieceType::WPawn);

	EXPECT_CALL(*mMockObserver, onRemoveLastCapturedPiece(Side::Black, PieceType::WPawn)).Times(1);
	mBlackPlayer->removeLastCapturedPiece();
}

} // namespace PlayerTests
