/*
  ==============================================================================
	Module:			En Passant Tests
	Description:    Testing the en passant special move
  ==============================================================================
*/

#include <gtest/gtest.h>

#include "MoveGeneration.h"
#include "MoveExecution.h"
#include "MoveValidation.h"


class EnPassantTests : public ::testing::Test
{
protected:
	std::shared_ptr<ChessBoard>		mBoard;
	std::shared_ptr<MoveValidation> mValidation;
	std::shared_ptr<MoveExecution>	mExecution;
	std::shared_ptr<MoveGeneration> mGeneration;

	void							SetUp() override
	{
		// Generate modules
		mBoard = std::make_shared<ChessBoard>();
		mBoard->initializeBoard();
		mValidation = std::make_shared<MoveValidation>(mBoard);
		mExecution	= std::make_shared<MoveExecution>(mBoard, mValidation);
		mGeneration = std::make_shared<MoveGeneration>(mBoard, mValidation, mExecution);
	}
};


TEST_F(EnPassantTests, EnPassantAvailableAfterDoublePawnMove)
{
	// Prepare board for en passant move
	// Clear board to setup a position for en passant
	mBoard->removeAllPiecesFromBoard();

	// White Pawn at e5 (4,3) and black pawn at d7 (3,1)
	Position whitePawnPos = {4, 3};
	Position blackPawnPos = {3, 1};
	mBoard->setPiece(whitePawnPos, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::White));
	mBoard->setPiece(blackPawnPos, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::Black));

	// Execute double pawn push with black pawn from d7 to d5
	PossibleMove doublePawnMove;
	doublePawnMove.start = blackPawnPos;
	doublePawnMove.end	 = {3, 1}; // d5
	doublePawnMove.type	 = MoveType::DoublePawnPush;

	mExecution->executeMove(doublePawnMove);

	// Check if white pawn at e5 has en passant move available
	mGeneration->calculateAllLegalBasicMoves(PlayerColor::White);
	auto moves			  = mGeneration->getMovesForPosition(whitePawnPos);

	bool hasEnPassantMove = false;
	for (const auto &move : moves)
	{
		if ((move.type & MoveType::EnPassant) == MoveType::EnPassant)
		{
			hasEnPassantMove = true;

			// Check if the move is to d6
			EXPECT_EQ(move.end.x, 3);
			EXPECT_EQ(move.end.y, 2);
			break;
		}
	}

	EXPECT_TRUE(hasEnPassantMove);
}


TEST_F(EnPassantTests, EnPassantCapturesCorrectly)
{
	// Prepare board for en passant move
	// Clear board to setup a position for en passant
	mBoard->removeAllPiecesFromBoard();

	// White Pawn at e5 (4,3) and black pawn at d7 (3,1)
	Position whitePawnPos = {4, 3};
	Position blackPawnPos = {3, 1};
	mBoard->setPiece(whitePawnPos, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::White));
	mBoard->setPiece(blackPawnPos, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::Black));

	// Execute Double pawn push
	PossibleMove doublePawnMove;
	doublePawnMove.start = blackPawnPos;
	doublePawnMove.end	 = Position{3, 3}; // d5
	doublePawnMove.type	 = MoveType::DoublePawnPush;

	mExecution->executeMove(doublePawnMove);

	// Execute EnPassant capture
	PossibleMove enPassantMove;
	enPassantMove.start = whitePawnPos;
	enPassantMove.end	= Position{3, 2}; // d6
	enPassantMove.type	= MoveType::EnPassant;

	mExecution->executeMove(enPassantMove);

	// Verify: White pawn has moved to d6
	EXPECT_FALSE(mBoard->isEmpty(Position{3, 2}));
	auto pieceAtD6 = mBoard->getPiece(Position{3, 2});
	EXPECT_EQ(pieceAtD6->getColor(), PlayerColor::White);
	EXPECT_EQ(pieceAtD6->getType(), PieceType::Pawn);

	// Verify: Black pawn at D5 has been captured (removed)
	EXPECT_TRUE(mBoard->isEmpty(Position{3, 3}));

	// Verify: Original white pawn position is empty
	EXPECT_TRUE(mBoard->isEmpty(whitePawnPos));
}


TEST_F(EnPassantTests, EnPassantOnlyAvailableImmediately)
{
	// Prepare board for en passant move
	// Clear board to setup a position for en passant
	mBoard->removeAllPiecesFromBoard();

	// White Pawn at e5 (4,3), h5 (7,3) and black pawn at d7 (3,1)
	Position whitePawnPos = {4, 3};
	Position otherWhitePawnPos{7, 3};
	Position blackPawnPos = {3, 1};
	mBoard->setPiece(whitePawnPos, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::White));
	mBoard->setPiece(blackPawnPos, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::Black));
	mBoard->setPiece(otherWhitePawnPos, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::White));

	// Execute double pawn move with black pawn (d7 to d5)
	PossibleMove doublePawnMove;
	doublePawnMove.start = blackPawnPos;
	doublePawnMove.end	 = Position{3, 3}; // d5
	doublePawnMove.type	 = MoveType::DoublePawnPush;

	mExecution->executeMove(doublePawnMove);

	// Make a different move (not capturing en passant)
	PossibleMove otherMove;
	otherMove.start = otherWhitePawnPos;
	otherMove.end	= Position{7, 2}; // h6
	otherMove.type	= MoveType::Normal;

	mExecution->executeMove(otherMove);

	// Verify: White pawn at e5 has no en passant available
	mGeneration->calculateAllLegalBasicMoves(PlayerColor::White);

	auto moves			  = mGeneration->getMovesForPosition(whitePawnPos);

	bool hasEnPassantMove = false;
	for (const auto &move : moves)
	{
		if ((move.type & MoveType::EnPassant) == MoveType::EnPassant)
		{
			hasEnPassantMove = true;
			break;
		}
	}

	EXPECT_FALSE(hasEnPassantMove);
}


TEST_F(EnPassantTests, EnPassantNotAvailableWithoutDoublePawnMove)
{
	// Clear board and set up position where en passant would not be possible
	mBoard->removeAllPiecesFromBoard();

	// Place white pawn at e5 (4,3), black pawn at d5 (3,3) directly
	Position whitePawnPos = {4, 3};
	Position blackPawnPos = {3, 3};
	mBoard->setPiece(whitePawnPos, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::White));
	mBoard->setPiece(blackPawnPos, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::Black));

	// Black pawn is already at d5 -> no double push happened

	// Verify: white pawn at e5 has no en passant move available
	mGeneration->calculateAllLegalBasicMoves(PlayerColor::White);
	auto moves			  = mGeneration->getMovesForPosition(whitePawnPos);

	bool hasEnPassantMove = false;
	for (const auto &move : moves)
	{
		if ((move.type & MoveType::EnPassant) == MoveType::EnPassant)
		{
			hasEnPassantMove = true;
			break;
		}
	}

	EXPECT_FALSE(hasEnPassantMove);
}