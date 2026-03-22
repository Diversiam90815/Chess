/*
  ==============================================================================
	Module:			Move Generation Tests
	Description:    Testing the general move generation module from the chess engine
  ==============================================================================
*/

#include <gtest/gtest.h>

#include "Generation/MoveGeneration.h"


namespace MoveTests
{

class MoveGenerationTest : public ::testing::Test
{
protected:
	Chessboard	   mBoard;
	MoveGeneration mGeneration{mBoard};


	void		   SetUp() override { mBoard.init(); }

	bool		   hasMoveTo(const MoveList &moves, Square to) const
	{
		for (size_t i = 0; i < moves.size(); ++i)
		{
			if (moves[i].to() == to)
				return true;
		}
		return false;
	}

	bool hasMoveFromTo(const MoveList &moves, Square from, Square to) const
	{
		for (size_t i = 0; i < moves.size(); ++i)
		{
			if (moves[i].from() == from && moves[i].to() == to)
				return true;
		}
		return false;
	}

	size_t countMovesFrom(const MoveList &moves, Square from) const
	{
		size_t count = 0;
		for (size_t i = 0; i < moves.size(); ++i)
		{
			if (moves[i].from() == from)
				++count;
		}
		return count;
	}
};


TEST_F(MoveGenerationTest, GenerateAllMovesReturnsNonEmpty)
{
	MoveList moves;
	mGeneration.generateAllMoves(moves);

	EXPECT_FALSE(moves.empty()) << "Should generate moves from initial position";
}


TEST_F(MoveGenerationTest, InitialPositionHas20Moves)
{
	MoveList moves;
	mGeneration.generateAllMoves(moves);

	// Initial position has 20 legal moves: 16 pawn moves + 4 knight moves
	EXPECT_EQ(moves.size(), 20) << "Initial position should have 20 pseudo-legal moves";
}


TEST_F(MoveGenerationTest, PawnInitialMovesCalculatedCorrectly)
{
	MoveList moves;
	mGeneration.generateAllMoves(moves);

	// Test pawn at e2 - should have exactly two moves (e3 and e4)
	size_t e2Moves = countMovesFrom(moves, Square::e2);
	EXPECT_EQ(e2Moves, 2) << "Pawn at e2 should have exactly two moves (e3 and e4)";

	EXPECT_TRUE(hasMoveFromTo(moves, Square::e2, Square::e3)) << "Pawn should be able to move e2->e3";
	EXPECT_TRUE(hasMoveFromTo(moves, Square::e2, Square::e4)) << "Pawn should be able to move e2->e4";
}


TEST_F(MoveGenerationTest, PawnCapturesCalculatedCorrectly)
{
	mBoard.clear();

	// Set up position with white pawn at e4 and black pieces at d5 and f5
	mBoard.addPiece(PieceType::WPawn, Square::e4);
	mBoard.addPiece(PieceType::BPawn, Square::d5);
	mBoard.addPiece(PieceType::BPawn, Square::f5);
	mBoard.addPiece(PieceType::WKing, Square::e1); // Need king for legal position
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	MoveList moves;
	mGeneration.generateAllMoves(moves);

	// Pawn should have: forward move (e5), and two captures (d5, f5)
	EXPECT_TRUE(hasMoveFromTo(moves, Square::e4, Square::e5)) << "Pawn should move forward to e5";
	EXPECT_TRUE(hasMoveFromTo(moves, Square::e4, Square::d5)) << "Pawn should capture on d5";
	EXPECT_TRUE(hasMoveFromTo(moves, Square::e4, Square::f5)) << "Pawn should capture on f5";
}


TEST_F(MoveGenerationTest, BlockedPawnGeneratesNoForwardMove)
{
	mBoard.clear();

	// White pawn at e4 blocked by a piece at e5
	mBoard.addPiece(PieceType::WPawn, Square::e4);
	mBoard.addPiece(PieceType::BPawn, Square::e5);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	MoveList moves;
	mGeneration.generateAllMoves(moves);

	// Verify: Pawn has no forward move since it's been blocked
	EXPECT_FALSE(hasMoveFromTo(moves, Square::e4, Square::e5)) << "Blocked pawn should not be able to move forward";
}


TEST_F(MoveGenerationTest, KnightMovesCalculatedCorrectly)
{
	mBoard.clear();

	// Place Knight on empty board at e4
	mBoard.addPiece(PieceType::WKnight, Square::e4);
	mBoard.addPiece(PieceType::WKing, Square::a1);
	mBoard.addPiece(PieceType::BKing, Square::h8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	MoveList moves;
	mGeneration.generateAllMoves(moves);

	// Knight in center should have 8 L-shaped moves
	size_t knightMoves = countMovesFrom(moves, Square::e4);
	EXPECT_EQ(knightMoves, 8) << "Knight in the center should have 8 possible L-shaped moves";

	// Check specific knight moves
	EXPECT_TRUE(hasMoveFromTo(moves, Square::e4, Square::c3)) << "Knight should move to c3";
	EXPECT_TRUE(hasMoveFromTo(moves, Square::e4, Square::c5)) << "Knight should move to c5";
	EXPECT_TRUE(hasMoveFromTo(moves, Square::e4, Square::d2)) << "Knight should move to d2";
	EXPECT_TRUE(hasMoveFromTo(moves, Square::e4, Square::d6)) << "Knight should move to d6";
	EXPECT_TRUE(hasMoveFromTo(moves, Square::e4, Square::f2)) << "Knight should move to f2";
	EXPECT_TRUE(hasMoveFromTo(moves, Square::e4, Square::f6)) << "Knight should move to f6";
	EXPECT_TRUE(hasMoveFromTo(moves, Square::e4, Square::g3)) << "Knight should move to g3";
	EXPECT_TRUE(hasMoveFromTo(moves, Square::e4, Square::g5)) << "Knight should move to g5";
}


TEST_F(MoveGenerationTest, KnightAtCornerHasLimitedMoves)
{
	mBoard.clear();

	// Place knight at corner a1
	mBoard.addPiece(PieceType::WKnight, Square::a1);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	MoveList moves;
	mGeneration.generateAllMoves(moves);

	// Knight in corner should have 2 moves
	size_t knightMoves = countMovesFrom(moves, Square::a1);
	EXPECT_EQ(knightMoves, 2) << "Knight in corner should have 2 moves";

	EXPECT_TRUE(hasMoveFromTo(moves, Square::a1, Square::b3)) << "Knight should move to b3";
	EXPECT_TRUE(hasMoveFromTo(moves, Square::a1, Square::c2)) << "Knight should move to c2";
}


TEST_F(MoveGenerationTest, BishopMovesCalculatedCorrectly)
{
	mBoard.clear();

	// Place Bishop on an empty board at e4
	mBoard.addPiece(PieceType::WBishop, Square::e4);
	mBoard.addPiece(PieceType::WKing, Square::a1);
	mBoard.addPiece(PieceType::BKing, Square::a8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	MoveList moves;
	mGeneration.generateAllMoves(moves);

	// Bishop in center should have 13 diagonal moves
	size_t bishopMoves = countMovesFrom(moves, Square::e4);
	EXPECT_EQ(bishopMoves, 13) << "Bishop in the center should have 13 diagonal moves";
}


TEST_F(MoveGenerationTest, BishopBlockedMovesCalculatedCorrectly)
{
	mBoard.clear();

	// Place bishop at e4 with blocking pieces
	mBoard.addPiece(PieceType::WBishop, Square::e4);
	mBoard.addPiece(PieceType::WPawn, Square::c6); // Blocks northwest
	mBoard.addPiece(PieceType::BPawn, Square::f5); // Can be captured (northeast)
	mBoard.addPiece(PieceType::WKing, Square::a1);
	mBoard.addPiece(PieceType::BKing, Square::h8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	MoveList moves;
	mGeneration.generateAllMoves(moves);

	// Should be able to capture at f5
	EXPECT_TRUE(hasMoveFromTo(moves, Square::e4, Square::f5)) << "Bishop should be able to capture enemy piece at f5";

	// Should not be able to move beyond c6 (blocked by own piece)
	EXPECT_FALSE(hasMoveFromTo(moves, Square::e4, Square::b7)) << "Bishop should not move beyond blocking piece at c6";

	// Should be able to move to d5 (between bishop and blocking piece)
	EXPECT_TRUE(hasMoveFromTo(moves, Square::e4, Square::d5)) << "Bishop should move to d5";
}


TEST_F(MoveGenerationTest, RookMovesCalculatedCorrectly)
{
	mBoard.clear();

	// Place rook on empty board at e4
	mBoard.addPiece(PieceType::WRook, Square::e4);
	mBoard.addPiece(PieceType::WKing, Square::a1);
	mBoard.addPiece(PieceType::BKing, Square::a8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	MoveList moves;
	mGeneration.generateAllMoves(moves);

	// Rook at e4 should have 14 straight-line moves
	size_t rookMoves = countMovesFrom(moves, Square::e4);
	EXPECT_EQ(rookMoves, 14) << "Rook in the center should have 14 straight-line moves";
}


TEST_F(MoveGenerationTest, QueenMovesCalculatedCorrectly)
{
	mBoard.clear();

	// Queen on an empty board at e4
	mBoard.addPiece(PieceType::WQueen, Square::e4);
	mBoard.addPiece(PieceType::WKing, Square::a1);
	mBoard.addPiece(PieceType::BKing, Square::a8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	MoveList moves;
	mGeneration.generateAllMoves(moves);

	// Queen at e4 should have 27 moves (14 rook + 13 bishop)
	size_t queenMoves = countMovesFrom(moves, Square::e4);
	EXPECT_EQ(queenMoves, 27) << "Queen in the center should have 27 moves";
}


TEST_F(MoveGenerationTest, KingMovesCalculatedCorrectly)
{
	mBoard.clear();

	// Place King in the middle of the board at e4
	mBoard.addPiece(PieceType::WKing, Square::e4);
	mBoard.addPiece(PieceType::BKing, Square::a8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	MoveList moves;
	mGeneration.generateAllMoves(moves);

	// King in center should have 8 moves (one in each direction)
	size_t kingMoves = countMovesFrom(moves, Square::e4);
	EXPECT_EQ(kingMoves, 8) << "King in the center should have 8 moves";
}


TEST_F(MoveGenerationTest, IsSquareAttackedByPawn)
{
	mBoard.clear();

	mBoard.addPiece(PieceType::WPawn, Square::e4);
	mBoard.addPiece(PieceType::WKing, Square::a1);
	mBoard.addPiece(PieceType::BKing, Square::h8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	// White pawn at e4 attacks d5 and f5
	EXPECT_TRUE(mGeneration.isSquareAttacked(Square::d5, Side::White)) << "d5 should be attacked by white pawn";
	EXPECT_TRUE(mGeneration.isSquareAttacked(Square::f5, Side::White)) << "f5 should be attacked by white pawn";
	EXPECT_FALSE(mGeneration.isSquareAttacked(Square::e5, Side::White)) << "e5 should not be attacked by pawn (forward is not attack)";
}


TEST_F(MoveGenerationTest, IsSquareAttackedByKnight)
{
	mBoard.clear();

	mBoard.addPiece(PieceType::WKnight, Square::e4);
	mBoard.addPiece(PieceType::WKing, Square::a1);
	mBoard.addPiece(PieceType::BKing, Square::h8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	EXPECT_TRUE(mGeneration.isSquareAttacked(Square::d6, Side::White)) << "d6 should be attacked by knight";
	EXPECT_TRUE(mGeneration.isSquareAttacked(Square::f6, Side::White)) << "f6 should be attacked by knight";
	EXPECT_FALSE(mGeneration.isSquareAttacked(Square::e5, Side::White)) << "e5 should not be attacked by knight";
}


TEST_F(MoveGenerationTest, CastlingMovesGenerated)
{
	mBoard.clear();

	// Set up castling position
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::WRook, Square::h1);
	mBoard.addPiece(PieceType::WRook, Square::a1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.setCastlingRights(Castling::WK | Castling::WQ);
	mBoard.updateOccupancies();

	MoveList moves;
	mGeneration.generateAllMoves(moves);

	// Should have castling moves
	bool hasKingsideCastle	= false;
	bool hasQueensideCastle = false;

	for (size_t i = 0; i < moves.size(); ++i)
	{
		if (moves[i].from() == Square::e1 && moves[i].to() == Square::g1)
			hasKingsideCastle = true;
		if (moves[i].from() == Square::e1 && moves[i].to() == Square::c1)
			hasQueensideCastle = true;
	}

	EXPECT_TRUE(hasKingsideCastle) << "Kingside castling should be generated";
	EXPECT_TRUE(hasQueensideCastle) << "Queenside castling should be generated";
}


TEST_F(MoveGenerationTest, EnPassantCaptureGenerated)
{
	mBoard.clear();

	// Set up en passant position: white pawn on e5, black pawn just moved d7-d5
	mBoard.addPiece(PieceType::WPawn, Square::e5);
	mBoard.addPiece(PieceType::BPawn, Square::d5);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.setEnPassantSquare(Square::d6); // Black just played d7-d5
	mBoard.updateOccupancies();

	MoveList moves;
	mGeneration.generateAllMoves(moves);

	// Should have en passant capture
	bool hasEnPassant = false;
	for (size_t i = 0; i < moves.size(); ++i)
	{
		if (moves[i].from() == Square::e5 && moves[i].to() == Square::d6 && moves[i].isEnPassant())
		{
			hasEnPassant = true;
			break;
		}
	}

	EXPECT_TRUE(hasEnPassant) << "En passant capture should be generated";
}


TEST_F(MoveGenerationTest, PawnPromotionMovesGenerated)
{
	mBoard.clear();

	// White pawn on 7th rank ready to promote
	mBoard.addPiece(PieceType::WPawn, Square::e7);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::a8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	MoveList moves;
	mGeneration.generateAllMoves(moves);

	// Count promotion moves (should have 4: Q, R, B, N)
	size_t promotionCount = 0;
	for (size_t i = 0; i < moves.size(); ++i)
	{
		if (moves[i].from() == Square::e7 && moves[i].to() == Square::e8 && moves[i].isPromotion())
		{
			++promotionCount;
		}
	}

	EXPECT_EQ(promotionCount, 4) << "Should generate 4 promotion moves (Q, R, B, N)";
}


} // namespace MoveTests
