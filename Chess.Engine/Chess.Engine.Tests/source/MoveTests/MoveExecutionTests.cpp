/*
  ==============================================================================
	Module:			Move Execution Tests
	Description:    Testing the general move execution module from the chess engine
  ==============================================================================
*/

#include <gtest/gtest.h>

#include "MoveExecution.h"
#include "MoveValidation.h"


class MoveExecutionTest : public ::testing::Test
{
protected:
	std::shared_ptr<ChessBoard>		mBoard;
	std::shared_ptr<MoveValidation> mValidation;
	std::shared_ptr<MoveExecution>	mExecution;

	void							SetUp() override
	{
		mBoard = std::make_shared<ChessBoard>();
		mBoard->initializeBoard();
		mValidation = std::make_shared<MoveValidation>(mBoard);
		mExecution	= std::make_shared<MoveExecution>(mBoard, mValidation);
	}
};


TEST_F(MoveExecutionTest, ExecuteMoveUpdatesBoard)
{
	// Move white pawn e2 to e4
	Position	 start = {4, 6};
	Position	 end   = {4, 4};
	PossibleMove move{start, end, MoveType::Normal, PieceType::DefaultType};

	auto		 result = mExecution->executeMove(move);

	// The pawn should now be at e4
	auto		 piece	= mBoard->getPiece(end);

	ASSERT_NE(piece, nullptr) << "Expected a piece at the destination";
	EXPECT_EQ(piece->getType(), PieceType::Pawn) << "Expected a pawn at the destination";
	EXPECT_EQ(piece->getColor(), PlayerColor::White) << "Expected a white piece at the destination";
}


TEST_F(MoveExecutionTest, ExecuteCaptureMove)
{
	mBoard->removeAllPiecesFromBoard();

	// Place white knight at e4, black pawn at f5
	Position whiteKnightPos = {4, 4}; // e4
	Position blackPawnPos	= {5, 3}; // f5

	mBoard->setPiece(whiteKnightPos, ChessPiece::CreatePiece(PieceType::Knight, PlayerColor::White));
	mBoard->setPiece(blackPawnPos, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::Black));

	// Execute capture move
	PossibleMove captureMove{whiteKnightPos, blackPawnPos, MoveType::Capture, PieceType::DefaultType};
	auto		 result	   = mExecution->executeMove(captureMove);

	// Verify: White knight now at f5, no piece at e4
	auto		 pieceAtF5 = mBoard->getPiece(blackPawnPos);
	ASSERT_NE(pieceAtF5, nullptr) << "Expected the capturing knight at destination";
	EXPECT_EQ(pieceAtF5->getType(), PieceType::Knight) << "Expected a knight at the destination";
	EXPECT_EQ(pieceAtF5->getColor(), PlayerColor::White) << "Expected a white piece at the destination";
	EXPECT_TRUE(mBoard->isEmpty(whiteKnightPos)) << "Original position should be empty after capture";

	// Verify: capture was recorded in the move
	const Move *lastMove = mExecution->getLastMove();
	ASSERT_NE(lastMove, nullptr) << "Expected a move in the history";
	EXPECT_EQ(lastMove->capturedPiece, PieceType::Pawn) << "Move should record the captured pawn";
}


TEST_F(MoveExecutionTest, AddAndRemoveFromMoveHistory)
{
	// Execute the move
	Position	 start = {4, 1}; // e7
	Position	 end   = {4, 3}; // e5
	PossibleMove move{start, end, MoveType::Normal, PieceType::DefaultType};
	Move		 executedMove(move);

	// Check for move added to move history
	mExecution->addMoveToHistory(executedMove);
	EXPECT_NE(mExecution->getLastMove(), nullptr) << "Last move should not be null after adding to history";

	// Check for removing the move
	mExecution->removeLastMove();
	EXPECT_EQ(mExecution->getLastMove(), nullptr) << "Last move should be null after removing from history";
}


TEST_F(MoveExecutionTest, MoveIncrementsPieceCounter)
{
	// Get the white pawn at e2
	Position	 start			  = {4, 6}; // e2
	auto		 pawn			  = mBoard->getPiece(start);
	int			 initialMoveCount = pawn->getMoveCounter();

	// Execute pawn move
	Position	 end			  = {4, 4}; // e4
	PossibleMove move{start, end, MoveType::Normal, PieceType::DefaultType};

	mExecution->executeMove(move);

	// Verify: the move counter was incremented
	auto movedPawn		= mBoard->getPiece(end);
	int	 newMoveCounter = movedPawn->getMoveCounter();

	EXPECT_EQ(newMoveCounter, initialMoveCount + 1) << "Piece move counter should be incremented after a move";
}

TEST_F(MoveExecutionTest, HalfMoveClockIncreaseForNonPawnNonCaptureMove)
{
	mBoard->removeAllPiecesFromBoard();

	// Setup white king at e1 und white knight at b1
	Position kingPos   = {4, 7}; // e1
	Position knightPos = {1, 7}; // b1

	mBoard->setPiece(kingPos, ChessPiece::CreatePiece(PieceType::King, PlayerColor::White));
	mBoard->setPiece(knightPos, ChessPiece::CreatePiece(PieceType::Knight, PlayerColor::White));
	mBoard->updateKingsPosition(kingPos, PlayerColor::White);

	// Execute knight move
	PossibleMove knightMove{knightPos, {2, 5}, MoveType::Normal, PieceType::DefaultType}; // b1 to c3
	auto		 result = mExecution->executeMove(knightMove);

	// Verify: half move clock increased
	EXPECT_GT(result.halfMoveClock, 0) << "Half-move clock should be incremented for non-pawn, non-capture move";
}


TEST_F(MoveExecutionTest, HalfMoveClockResetsForPawnMove)
{
	mBoard->removeAllPiecesFromBoard();

	// Setup white knight at e1, white pawn at e2 and white knight at b1
	Position kingPos   = {4, 7}; // e1
	Position knightPos = {1, 7}; // b1
	Position pawnPos   = {4, 6}; // e2

	mBoard->setPiece(kingPos, ChessPiece::CreatePiece(PieceType::King, PlayerColor::White));
	mBoard->setPiece(knightPos, ChessPiece::CreatePiece(PieceType::Knight, PlayerColor::White));
	mBoard->setPiece(pawnPos, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::White));
	mBoard->updateKingsPosition(kingPos, PlayerColor::White);

	// First move knight (increase half move clock)
	PossibleMove knightMove{knightPos, {2, 5}, MoveType::Normal, PieceType::DefaultType}; // b1 to c3
	mExecution->executeMove(knightMove);

	// Then move pawn
	PossibleMove pawnMove{pawnPos, {4, 4}, MoveType::Normal, PieceType::DefaultType}; // e2 to e4
	auto		 result = mExecution->executeMove(pawnMove);

	// Verify: half move clock was reset
	EXPECT_EQ(result.halfMoveClock, 0) << "Half-move clock should be reset to 0 for pawn move";
}


TEST_F(MoveExecutionTest, HalfMoveClockResetsForCaptureMove)
{
	mBoard->removeAllPiecesFromBoard();

	// Setup a position for a capture
	Position kingPos		= {4, 7}; // e1
	Position whiteKnightPos = {1, 7}; // b1
	Position blackPawnPos	= {2, 5}; // c3

	mBoard->setPiece(kingPos, ChessPiece::CreatePiece(PieceType::King, PlayerColor::White));
	mBoard->setPiece(whiteKnightPos, ChessPiece::CreatePiece(PieceType::Knight, PlayerColor::White));
	mBoard->setPiece(blackPawnPos, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::Black));
	mBoard->updateKingsPosition(kingPos, PlayerColor::White);

	// Knight captures pawn
	PossibleMove captureMove{whiteKnightPos, blackPawnPos, MoveType::Capture, PieceType::DefaultType};
	auto		 result = mExecution->executeMove(captureMove);

	// Verify: half move clock was reset
	EXPECT_EQ(result.halfMoveClock, 0) << "Half-move clock should reset to 0 for capture move";
}


TEST_F(MoveExecutionTest, MoveNumberIncrements)
{
	// Execute two moves
	Position	 start1 = {4, 6}; // e2
	Position	 end1	= {4, 4}; // e4
	PossibleMove move1{start1, end1, MoveType::Normal, PieceType::DefaultType};
	auto		 result1 = mExecution->executeMove(move1);

	Position	 start2	 = {5, 1}; // f7
	Position	 end2	 = {5, 3}; // f5
	PossibleMove move2{start2, end2, MoveType::Normal, PieceType::DefaultType};
	auto		 result2 = mExecution->executeMove(move2);

	// Verify: move numbers were incremented correctly
	EXPECT_EQ(result1.number, 1) << "First move should have number 1";
	EXPECT_EQ(result2.number, 2) << "Second move should have number 2";
}


TEST_F(MoveExecutionTest, CheckFlagAddedWhenMovingIntoCheck)
{
	mBoard->removeAllPiecesFromBoard();

	// Setup: White queen about to check black king
	Position whiteQueenPos = {3, 7}; // d1
	Position blackKingPos  = {3, 0}; // d8

	mBoard->setPiece(whiteQueenPos, ChessPiece::CreatePiece(PieceType::Queen, PlayerColor::White));
	mBoard->setPiece(blackKingPos, ChessPiece::CreatePiece(PieceType::King, PlayerColor::Black));
	mBoard->updateKingsPosition(blackKingPos, PlayerColor::Black);

	// Execute move that puts king in check
	PossibleMove checkMove{whiteQueenPos, {3, 4}, MoveType::Normal, PieceType::DefaultType}; // d1 to d4
	auto		 result	   = mExecution->executeMove(checkMove);

	bool		 checkFlag = (result.type & MoveType::Check) == MoveType::Check;

	// Verify: check flag was added
	EXPECT_TRUE(checkFlag) << "Move should include Check flag when putting opponent's king in check";
}


TEST_F(MoveExecutionTest, PlayerColorIsCorrectlyRecorded)
{
	// Move a white pawn
	Position	 startWhite = {4, 6}; // e2
	Position	 endWhite	= {4, 4}; // e4
	PossibleMove moveWhite{startWhite, endWhite, MoveType::Normal, PieceType::DefaultType};
	auto		 resultWhite = mExecution->executeMove(moveWhite);

	// Move a black pawn
	Position	 startBlack	 = {4, 1}; // e7
	Position	 endBlack	 = {4, 3}; // e5
	PossibleMove moveBlack{startBlack, endBlack, MoveType::Normal, PieceType::DefaultType};
	auto		 resultBlack = mExecution->executeMove(moveBlack);

	// Verify: player color was correctly recorded
	EXPECT_EQ(resultWhite.player, PlayerColor::White) << "White move should record White player";
	EXPECT_EQ(resultBlack.player, PlayerColor::Black) << "Black move should record Black player";
}


TEST_F(MoveExecutionTest, MovedAndCapturedPieceTypesAreRecorded)
{
	mBoard->removeAllPiecesFromBoard();

	// Place white knight and black pawn
	Position knightPos = {4, 4}; // e4
	Position pawnPos   = {5, 3}; // f5

	mBoard->setPiece(knightPos, ChessPiece::CreatePiece(PieceType::Knight, PlayerColor::White));
	mBoard->setPiece(pawnPos, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::Black));

	// Execute capture
	PossibleMove captureMove{knightPos, pawnPos, MoveType::Capture, PieceType::DefaultType};
	auto		 result = mExecution->executeMove(captureMove);

	// Verify: piece types were correctly recorded
	EXPECT_EQ(result.movedPiece, PieceType::Knight) << "Moved piece should be recorded as Knight";
	EXPECT_EQ(result.capturedPiece, PieceType::Pawn) << "Captured piece should be recorded as Pawn";
}


TEST_F(MoveExecutionTest, MoveTypeIsPreservedInMove)
{
	// Setup a normal move
	Position	 start = {4, 6}; // e2
	Position	 end   = {4, 4}; // e4
	PossibleMove possibleMove{start, end, MoveType::Normal, PieceType::DefaultType};

	// Execute the move
	auto		 result = mExecution->executeMove(possibleMove);

	// Verify the move type was preserved
	EXPECT_EQ(result.type, MoveType::Normal) << "Move type should be preserved in executed move";
}


TEST_F(MoveExecutionTest, ExecutingMoveGeneratesNotation)
{
	// Make a standard opening move (e2->e4)
	Position	 start = {4, 6}; // e2
	Position	 end   = {4, 4}; // e4
	PossibleMove move{start, end, MoveType::Normal, PieceType::DefaultType};

	auto		 result = mExecution->executeMove(move);

	// Verify the notation is generated
	EXPECT_FALSE(result.notation.empty()) << "Move notation should be generated";
}

