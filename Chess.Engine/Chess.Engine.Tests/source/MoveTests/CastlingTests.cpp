/*
  ==============================================================================
	Module:			Castling Tests
	Description:    Testing the castling special move
  ==============================================================================
*/

#include <gtest/gtest.h>

#include "MoveGeneration.h"
#include "MoveExecution.h"
#include "MoveValidation.h"


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
