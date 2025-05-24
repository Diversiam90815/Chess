/*
  ==============================================================================
	Module:			Move Validation Tests
	Description:    Testing the general move validation module from the chess engine
  ==============================================================================
*/

#include <gtest/gtest.h>

#include "MoveValidation.h"
#include "TestHelper.h"


class MoveValidationTest : public ::testing::Test
{
protected:
	std::shared_ptr<ChessBoard>		mBoard;
	std::shared_ptr<MoveValidation> mValidation;


	void							SetUp() override
	{
		mBoard = std::make_shared<ChessBoard>();
		mBoard->initializeBoard();
		mValidation = std::make_shared<MoveValidation>(mBoard);
	}
};


TEST_F(MoveValidationTest, ValidateMoveReturnsTrueForLegalMove)
{
	// Move white pawn from e2 to e4
	Position start = {4, 1};
	Position end   = {4, 3};
	Move	 move(start, end, PieceType::Pawn);

	bool	 result = mValidation->validateMove(move, PlayerColor::White);

	EXPECT_TRUE(result) << "Valid move should return true";
}


TEST_F(MoveValidationTest, InitialSetupEndgameChecks)
{
	// Set parameters
	PlayerColor whitePlayer		   = PlayerColor::White;
	PlayerColor blackPlayer		   = PlayerColor::Black;

	Position	kingPosWhite	   = mBoard->getKingsPosition(whitePlayer);
	Position	kingPosBlack	   = mBoard->getKingsPosition(blackPlayer);

	bool		isKingInCheckWhite = mValidation->isKingInCheck(kingPosWhite, whitePlayer);
	bool		isKingInCheckBlack = mValidation->isKingInCheck(kingPosBlack, blackPlayer);

	bool		isCheckmateWhite   = mValidation->isCheckmate(whitePlayer);
	bool		isCheckmateBlack   = mValidation->isCheckmate(blackPlayer);

	bool		isStalemateWhite   = mValidation->isStalemate(whitePlayer);
	bool		isStalemateBlack   = mValidation->isStalemate(blackPlayer);

	// Verify: King should not be in check on initial board setup
	EXPECT_FALSE(isKingInCheckWhite) << "White King should not be in check on initial board";
	EXPECT_FALSE(isKingInCheckBlack) << "Black King should not be in check on initial board";

	// Verify: Validation should not detect checkmate on initial board setup
	EXPECT_FALSE(isCheckmateWhite) << "White should not be in checkmate on initial board";
	EXPECT_FALSE(isCheckmateBlack) << "Black should not be in checkmate on initial board";

	// Verify: Stalemate returns false on initial board setup
	EXPECT_FALSE(isStalemateWhite) << "White should not be in stalemate on initial board";
	EXPECT_FALSE(isStalemateBlack) << "Black should not be in stalemate on initial board";
}


TEST_F(MoveValidationTest, DetectsKingInCheckFromQueen)
{
	mBoard->removeAllPiecesFromBoard();

	// Setup white king at e1, black queen at e8 (in check along e-file)
	Position whiteKingPos  = {4, 7}; // e1
	Position blackQueenPos = {4, 0}; // e8

	mBoard->setPiece(whiteKingPos, ChessPiece::CreatePiece(PieceType::King, PlayerColor::White));
	mBoard->setPiece(blackQueenPos, ChessPiece::CreatePiece(PieceType::Queen, PlayerColor::Black));
	mBoard->updateKingsPosition(whiteKingPos, PlayerColor::White);

	// Check for check
	bool isKingInCheck = mValidation->isKingInCheck(whiteKingPos, PlayerColor::White);

	// Verify: King is in check
	EXPECT_TRUE(isKingInCheck) << " King should be in check from queen on same file";
}


TEST_F(MoveValidationTest, DetectsKingInCheckFromKnight)
{
	mBoard->removeAllPiecesFromBoard();

	// Setup white king at e1, black queen at c5 (in check)
	Position whiteKingPos	= {4, 7}; // e1
	Position blackKnightPos = {2, 3}; // c5

	mBoard->setPiece(whiteKingPos, ChessPiece::CreatePiece(PieceType::King, PlayerColor::White));
	mBoard->setPiece(blackKnightPos, ChessPiece::CreatePiece(PieceType::Knight, PlayerColor::Black));
	mBoard->updateKingsPosition(whiteKingPos, PlayerColor::White);

	// Check for check
	bool isKingInCheck = mValidation->isKingInCheck(whiteKingPos, PlayerColor::White);

	// Verify: King is in check
	EXPECT_TRUE(isKingInCheck) << " King should be in check from knight's L-shapred move";
}


TEST_F(MoveValidationTest, DetectsKingInCheckFromPawn)
{
	mBoard->removeAllPiecesFromBoard();

	// Setup white king at e1, black queen at c5 (in check)
	Position whiteKingPos = {4, 7}; // e1
	Position blackPawnPos = {3, 3}; // d5

	mBoard->setPiece(whiteKingPos, ChessPiece::CreatePiece(PieceType::King, PlayerColor::White));
	mBoard->setPiece(blackPawnPos, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::Black));
	mBoard->updateKingsPosition(whiteKingPos, PlayerColor::White);

	// Check for check
	bool isKingInCheck = mValidation->isKingInCheck(whiteKingPos, PlayerColor::White);

	// Verify: King is in check
	EXPECT_TRUE(isKingInCheck) << " King should be in check from pawn's diagonal attack";
}


TEST_F(MoveValidationTest, DetectScholarsMateCheckmate)
{
	mBoard->removeAllPiecesFromBoard();

	// Setup scholar's mate position:
	// Black king at e8, black pawns at standard positions
	// Wite queen at h5 (delivering checkmate)

	Position blackKingPos = {4, 0};															  // e8

	mBoard->setPiece(blackKingPos, ChessPiece::CreatePiece(PieceType::King, PlayerColor::Black));
	mBoard->setPiece({3, 1}, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::Black));	  // d7
	mBoard->setPiece({4, 1}, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::Black));	  // e7
	mBoard->setPiece({5, 1}, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::Black));	  // f7
	mBoard->setPiece({7, 3}, ChessPiece::CreatePiece(PieceType::Queen, PlayerColor::White));  // h5
	mBoard->setPiece({5, 2}, ChessPiece::CreatePiece(PieceType::Bishop, PlayerColor::White)); // f6

	mBoard->updateKingsPosition(blackKingPos, PlayerColor::Black);

	// Check for checkmate
	bool isBlackInCheckmate = mValidation->isCheckmate(PlayerColor::Black);

	// Verify: Black king is in checkmate
	EXPECT_TRUE(isBlackInCheckmate) << "Scholar's mate should be detected as checkmate";
}


TEST_F(MoveValidationTest, DetectsStalemate)
{
	mBoard->removeAllPiecesFromBoard();

	// Setup common stalemate position
	Position blackKingPos  = {7, 0}; // h8
	Position whiteQueenPos = {6, 2}; // g6

	mBoard->setPiece(blackKingPos, ChessPiece::CreatePiece(PieceType::King, PlayerColor::Black));
	mBoard->setPiece(whiteQueenPos, ChessPiece::CreatePiece(PieceType::Queen, PlayerColor::White));
	mBoard->updateKingsPosition(blackKingPos, PlayerColor::Black);

	// Check for stalemate
	bool isStalemateForBlack = mValidation->isStalemate(PlayerColor::Black);

	// Verify : Black is in stalemate
	EXPECT_TRUE(isStalemateForBlack) << "Position should be detected as stalemate - king has no legal moves but is not in check";
}


TEST_F(MoveValidationTest, DetectsPinnedPiece)
{
	mBoard->removeAllPiecesFromBoard();

	// Setup white king at e1, white bishop at d2, black rook at c3
	// -> bishop is pinned to the king

	Position whiteKingPos	= {4, 7}; // e1
	Position whiteBishopPos = {3, 6}; // d2
	Position blackRookPos	= {2, 5}; // c3

	mBoard->setPiece(whiteKingPos, ChessPiece::CreatePiece(PieceType::King, PlayerColor::White));
	mBoard->setPiece(whiteBishopPos, ChessPiece::CreatePiece(PieceType::Bishop, PlayerColor::White));
	mBoard->setPiece(blackRookPos, ChessPiece::CreatePiece(PieceType::Rook, PlayerColor::Black));
	mBoard->updateKingsPosition(whiteKingPos, PlayerColor::White);

	// Setup moves

	Move illegalMove(whiteBishopPos, {1, 4}, PieceType::Bishop); // Illegal move that put the bishop off the pin line
	Move legalMove(whiteBishopPos, {4, 5}, PieceType::Bishop);	 // Move along the pin line

	bool canMoveOutOfPin = mValidation->validateMove(illegalMove, PlayerColor::White);
	bool canMoveAlongPin = mValidation->validateMove(legalMove, PlayerColor::White);

	// Verify: Validation detects pinned piece
	EXPECT_FALSE(canMoveOutOfPin) << "Pinned bishop should not be allowed to move out of the pin line";
	EXPECT_TRUE(canMoveAlongPin) << "Pinned bishop should be allowed to move along the pin line";
}


TEST_F(MoveValidationTest, DetectsMoveIntoCheck)
{
	mBoard->removeAllPiecesFromBoard();

	// Setup: White king at e1, black rook at e8
	Position kingPos = {4, 7}; // e1
	Position rookPos = {4, 0}; // e8

	mBoard->setPiece(kingPos, ChessPiece::CreatePiece(PieceType::King, PlayerColor::White));
	mBoard->setPiece(rookPos, ChessPiece::CreatePiece(PieceType::Rook, PlayerColor::Black));
	mBoard->updateKingsPosition(kingPos, PlayerColor::White);

	// Setup moves
	Move illegalMove(kingPos, {4, 6}, PieceType::King); // Try to move the king to e2, which would still be in check
	Move legalMove(kingPos, {3, 7}, PieceType::King);	// Try to move the king to d1, which avoids check

	bool kingCanMoveIntoCheck  = mValidation->validateMove(illegalMove, PlayerColor::White);
	bool kingCanMoveOutOfCheck = mValidation->validateMove(legalMove, PlayerColor::White);

	// Verify: Cannot move into check, but out of it
	EXPECT_FALSE(kingCanMoveIntoCheck) << "King should not be allowed to move into check";
	EXPECT_TRUE(kingCanMoveOutOfCheck) << "King should be allowed to move out of check";
}


TEST_F(MoveValidationTest, CanCaptureCheckingPiece)
{
	mBoard->removeAllPiecesFromBoard();

	// Setup: White king at e1, white bishop at f2, black rook at e8 (checking the king)
	Position kingPos   = {4, 7}; // e1
	Position bishopPos = {5, 6}; // f2
	Position rookPos   = {4, 0}; // e8

	mBoard->setPiece(kingPos, ChessPiece::CreatePiece(PieceType::King, PlayerColor::White));
	mBoard->setPiece(bishopPos, ChessPiece::CreatePiece(PieceType::Bishop, PlayerColor::White));
	mBoard->setPiece(rookPos, ChessPiece::CreatePiece(PieceType::Rook, PlayerColor::Black));
	mBoard->updateKingsPosition(kingPos, PlayerColor::White);

	// Setup moves

	// Try to capture the checking rook with the bishop
	Move captureMoveByBishop(bishopPos, rookPos, PieceType::Bishop);
	captureMoveByBishop.type		  = MoveType::Capture;
	captureMoveByBishop.capturedPiece = PieceType::Rook;

	// Try to block the check with the bishop
	Move blockMove(bishopPos, {4, 3}, PieceType::Bishop); // f2 to e5

	// Check for capturing checking pieces
	bool isCaptureMoveValid	 = mValidation->validateMove(captureMoveByBishop, PlayerColor::White);
	bool isBlockingMoveValid = mValidation->validateMove(blockMove, PlayerColor::White);

	// Verify: Can capture checking piece
	EXPECT_FALSE(isCaptureMoveValid) << "Bishop shouldn't be able to teleport to capture the rook";
	EXPECT_TRUE(isBlockingMoveValid) << "Bishop should be able to block the check";
}

TEST_F(MoveValidationTest, CannotLeaveKingInCheck)
{
	mBoard->removeAllPiecesFromBoard();

	// Setup white king at e1, white queen at d1, black rook 14 e8 (checking the king)
	Position whiteKingPos  = {4, 7}; // e1
	Position whiteQueenPos = {3, 7}; // d1
	Position blackRookPos  = {4, 0}; // e8

	mBoard->setPiece(whiteKingPos, ChessPiece::CreatePiece(PieceType::King, PlayerColor::White));
	mBoard->setPiece(whiteQueenPos, ChessPiece::CreatePiece(PieceType::Queen, PlayerColor::White));
	mBoard->setPiece(blackRookPos, ChessPiece::CreatePiece(PieceType::Rook, PlayerColor::Black));
	mBoard->updateKingsPosition(whiteKingPos, PlayerColor::White);

	// Setup move for the queen, leaving king in check
	Move moveQueenAway(whiteQueenPos, {3, 0}, PieceType::Queen); // d1->d8

	bool isMoveValid = mValidation->validateMove(moveQueenAway, PlayerColor::White);

	// Verify: Move is invalid since king would be in check afterwards
	EXPECT_FALSE(isMoveValid) << "Queen should not be alowed to move away, leaving king in check";
}


TEST_F(MoveValidationTest, BlockingCheckmatePreventsCheckmate)
{
	mBoard->removeAllPiecesFromBoard();

	// Setup black king at h8, black rook at h7, white queen at g7 (near checkmate)
	Position blackKingPos  = {7, 0}; // h8
	Position blackRookPos  = {7, 1}; // h7
	Position whiteQueenPos = {6, 1}; // g7

	mBoard->setPiece(blackKingPos, ChessPiece::CreatePiece(PieceType::King, PlayerColor::Black));
	mBoard->setPiece(blackRookPos, ChessPiece::CreatePiece(PieceType::Rook, PlayerColor::Black));
	mBoard->setPiece(whiteQueenPos, ChessPiece::CreatePiece(PieceType::Queen, PlayerColor::White));
	mBoard->updateKingsPosition(blackKingPos, PlayerColor::Black);

	// Check for checkmate
	bool isCheckmate = mValidation->isCheckmate(PlayerColor::Black);

	// Verify: No checkmate, since it can be blocked by the rook
	EXPECT_FALSE(isCheckmate) << "Position should not be checkmate, because rook can block";
}
