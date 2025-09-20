/*
  ==============================================================================
	Module:			Castling Tests
	Description:    Testing the castling special move
  ==============================================================================
*/

#include <gtest/gtest.h>

#include "Generation/MoveGeneration.h"
#include "Validation/MoveValidation.h"
#include "Execution/MoveExecution.h"


namespace MoveTests
{

class CastlingTests : public ::testing::Test
{
protected:
	std::shared_ptr<ChessBoard>		mBoard;
	std::shared_ptr<MoveValidation> mValidation;
	std::shared_ptr<MoveExecution>	mExecution;
	std::shared_ptr<MoveGeneration> mGeneration;

	void							SetUp() override
	{
		mBoard = std::make_shared<ChessBoard>();
		mBoard->initializeBoard();
		mValidation = std::make_shared<MoveValidation>(mBoard);
		mExecution	= std::make_shared<MoveExecution>(mBoard, mValidation);
		mGeneration = std::make_shared<MoveGeneration>(mBoard, mValidation, mExecution);
	}

	void SetupCastlingPosition(PlayerColor color)
	{
		mBoard->removeAllPiecesFromBoard();

		int		 backRank = (color == PlayerColor::White) ? 7 : 0;

		// Place king and rooks
		Position kingPos{4, backRank};
		Position kingsideRookPos{7, backRank};
		Position queensideRookPos{0, backRank};

		mBoard->setPiece(kingPos, ChessPiece::CreatePiece(PieceType::King, color));
		mBoard->setPiece(kingsideRookPos, ChessPiece::CreatePiece(PieceType::Rook, color));
		mBoard->setPiece(queensideRookPos, ChessPiece::CreatePiece(PieceType::Rook, color));

		// Update king position
		mBoard->updateKingsPosition(kingPos, color);
	}
};


TEST_F(CastlingTests, KingsieCastlingMovesKingAndRook)
{
	SetupCastlingPosition(PlayerColor::White);

	// Execute kingside castling
	PossibleMove castlingMove;
	castlingMove.start = Position{4, 7}; // e1
	castlingMove.end   = Position{6, 7}; // g1
	castlingMove.type  = MoveType::CastlingKingside;

	mExecution->executeMove(castlingMove);

	// Verify: King has Position g1 (6,7)
	EXPECT_FALSE(mBoard->isEmpty(Position{6, 7})) << "King should be at g1";
	auto kingPiece = mBoard->getPiece(Position{6, 7});
	EXPECT_EQ(kingPiece->getType(), PieceType::King);
	EXPECT_EQ(kingPiece->getColor(), PlayerColor::White);

	// Verify: Rook has Position f1 (5,7)
	EXPECT_FALSE(mBoard->isEmpty(Position{5, 7})) << "Rook should be at f1";
	auto rookPiece = mBoard->getPiece(Position{5, 7});
	EXPECT_EQ(rookPiece->getColor(), PlayerColor::White);
	EXPECT_EQ(rookPiece->getType(), PieceType::Rook);

	// Verify: Original positions are empty
	EXPECT_TRUE(mBoard->isEmpty(Position{4, 7})) << "King's original position (e1) should be empty";
	EXPECT_TRUE(mBoard->isEmpty(Position{7, 7})) << "Rook's original position (h1) should be empty";
}


TEST_F(CastlingTests, QueensideCastlingMovesKingAndRook)
{
	SetupCastlingPosition(PlayerColor::White);

	// Execute queenside castling
	PossibleMove castlingMove;
	castlingMove.start = Position{4, 7}; // e1
	castlingMove.end   = Position{2, 7}; // c1
	castlingMove.type  = MoveType::CastlingQueenside;

	mExecution->executeMove(castlingMove);

	// Verify: king's new position is c1
	EXPECT_FALSE(mBoard->isEmpty(Position{2, 7})) << "King should be at c1";
	auto kingPiece = mBoard->getPiece(Position{2, 7});
	EXPECT_EQ(kingPiece->getType(), PieceType::King);
	EXPECT_EQ(kingPiece->getColor(), PlayerColor::White);

	// Verify: rook's new position is d1
	EXPECT_FALSE(mBoard->isEmpty(Position{3, 7})) << "Rook should be at d1";
	auto rookPiece = mBoard->getPiece(Position{3, 7});
	EXPECT_EQ(rookPiece->getType(), PieceType::Rook);
	EXPECT_EQ(rookPiece->getColor(), PlayerColor::White);

	// Verify: Orignal positions are empty
	EXPECT_TRUE(mBoard->isEmpty(Position{4, 7})) << "King's original position (e1) should be empty";
	EXPECT_TRUE(mBoard->isEmpty(Position{0, 7})) << "Rook's original position (a1) should be empty";
}


TEST_F(CastlingTests, CastlingBlockedByPieces)
{
	SetupCastlingPosition(PlayerColor::White);

	// Place a piece in between king and kingside rook
	mBoard->setPiece(Position{5, 7}, ChessPiece::CreatePiece(PieceType::Bishop, PlayerColor::White));

	// Calculate and get moves for king
	mGeneration->calculateAllLegalBasicMoves(PlayerColor::White);
	auto moves				 = mGeneration->getMovesForPosition(Position{4, 7});

	// Check the moves for kingside castling
	bool hasKingsideCastling = false;
	for (const auto &move : moves)
	{
		if ((move.type & MoveType::CastlingKingside) == MoveType::CastlingKingside)
		{
			hasKingsideCastling = true;
			break;
		}
	}

	// Verify: King has no Kingside castling move
	EXPECT_FALSE(hasKingsideCastling) << "Kingside castling should not be available when a piece blocks the path";

	// Check for queenside castling:

	// Place a piece between king and queenside rook
	mBoard->setPiece(Position{1, 7}, ChessPiece::CreatePiece(PieceType::Knight, PlayerColor::White));

	// Get moves for king position again
	mGeneration->calculateAllLegalBasicMoves(PlayerColor::White);
	moves					  = mGeneration->getMovesForPosition(Position{4, 7});

	// Check that queenside castling is not available
	bool hasQueensideCastling = false;
	for (const auto &move : moves)
	{
		if ((move.type & MoveType::CastlingQueenside) == MoveType::CastlingQueenside)
		{
			hasQueensideCastling = true;
			break;
		}
	}

	// Verify: King has no queenside castling move available
	EXPECT_FALSE(hasQueensideCastling) << "Queenside castling should not be available when a piece blocks the path";
}


TEST_F(CastlingTests, CastlingNotAvailableAfterKingMoves)
{
	SetupCastlingPosition(PlayerColor::White);

	// Move the king
	PossibleMove kingMove;
	kingMove.start = Position{4, 7}; // e1
	kingMove.end   = Position{4, 6}; // e2
	kingMove.type  = MoveType::Normal;

	mExecution->executeMove(kingMove);

	// Move the king back
	PossibleMove kingMoveBack;
	kingMoveBack.start = Position{4, 6}; // e2
	kingMoveBack.end   = Position{4, 7}; // e1
	kingMoveBack.type  = MoveType::Normal;

	mExecution->executeMove(kingMoveBack);

	// Get moves for king position
	mGeneration->calculateAllLegalBasicMoves(PlayerColor::White);
	auto moves		 = mGeneration->getMovesForPosition(Position{4, 7});

	// Check the moves for kingside castling
	bool hasCastling = false;
	for (const auto &move : moves)
	{
		if ((move.type & MoveType::CastlingKingside) == MoveType::CastlingKingside || (move.type & MoveType::CastlingQueenside) == MoveType::CastlingQueenside)
		{
			hasCastling = true;
			break;
		}
	}

	// Verify: Castling not available
	EXPECT_FALSE(hasCastling) << "Castling should not be available after king has moved";
}


TEST_F(CastlingTests, CastlingNotAvailableAfterRookMoves)
{
	SetupCastlingPosition(PlayerColor::White);

	// Move the kingside rook
	PossibleMove rookMove;
	rookMove.start = Position{7, 7}; // h1
	rookMove.end   = Position{7, 6}; // h2
	rookMove.type  = MoveType::Normal;

	mExecution->executeMove(rookMove);

	// Move the rook back
	PossibleMove rookMoveBack;
	rookMoveBack.start = Position{7, 6}; // h2
	rookMoveBack.end   = Position{7, 7}; // h1
	rookMoveBack.type  = MoveType::Normal;

	mExecution->executeMove(rookMoveBack);

	// Get moves for king position
	mGeneration->calculateAllLegalBasicMoves(PlayerColor::White);
	auto moves				 = mGeneration->getMovesForPosition(Position{4, 7});

	// Check the moves for kingside castling
	bool hasKingsideCastling = false;
	for (const auto &move : moves)
	{
		if ((move.type & MoveType::CastlingKingside) == MoveType::CastlingKingside)
		{
			hasKingsideCastling = true;
			break;
		}
	}

	// Verify: Kingside castling move not available
	EXPECT_FALSE(hasKingsideCastling) << "Kingside castling should not be available after rook has moved";
}


TEST_F(CastlingTests, CastlingNotAllowedThroughCheck)
{
	SetupCastlingPosition(PlayerColor::White);

	// Place an opponent piece that attacks a square the king must pass through
	// Black rook at f3 which controls f1, preventing kingside castling
	mBoard->setPiece(Position{5, 5}, ChessPiece::CreatePiece(PieceType::Rook, PlayerColor::Black));

	// Get moves for king position
	mGeneration->calculateAllLegalBasicMoves(PlayerColor::White);
	auto moves				 = mGeneration->getMovesForPosition(Position{4, 7});

	// Check that kingside castling is not available
	bool hasKingsideCastling = false;
	for (const auto &move : moves)
	{
		if ((move.type & MoveType::CastlingKingside) == MoveType::CastlingKingside)
		{
			hasKingsideCastling = true;
			break;
		}
	}

	// Verify: Castling move not available
	EXPECT_FALSE(hasKingsideCastling) << "Kingside castling should not be allowed through check";
}


TEST_F(CastlingTests, CastlingNotAllowedWhenKingInCheck)
{
	SetupCastlingPosition(PlayerColor::White);

	// Place an opponent piece that attacks the king
	// Black rook at e3 which checks the king on e1
	mBoard->setPiece(Position{4, 5}, ChessPiece::CreatePiece(PieceType::Rook, PlayerColor::Black));

	// Get moves for king position
	mGeneration->calculateAllLegalBasicMoves(PlayerColor::White);
	auto moves		 = mGeneration->getMovesForPosition(Position{4, 7});

	// Check that no castling is available
	bool hasCastling = false;
	for (const auto &move : moves)
	{
		if ((move.type & MoveType::CastlingKingside) == MoveType::CastlingKingside || (move.type & MoveType::CastlingQueenside) == MoveType::CastlingQueenside)
		{
			hasCastling = true;
			break;
		}
	}

	// Verify: Castling should not be available
	EXPECT_FALSE(hasCastling) << "Castling should not be allowed when king is in check";
}

} // namespace MoveTests