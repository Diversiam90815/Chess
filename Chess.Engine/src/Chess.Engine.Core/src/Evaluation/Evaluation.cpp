/*
  ==============================================================================
	Module:         Evaluation
	Description:    Positional Evaluation of the chess board
  ==============================================================================
*/

#include "Evaluation.h"


int Evaluation::evaluate(const Chessboard &board)
{
	int score = 0;

	score += evaluateMaterial(board);
	score += evaluatePieceSquareTables(board);
	score += evaluatePawnStructure(board);
	score += evaluateKingSafety(board);
	score += evaluateMobility(board);

	// Return score relative to side to move
	return (board.getCurrentSide() == Side::White) ? score : -score;
}


int Evaluation::evaluateMaterial(const Chessboard &board)
{
	const auto &pieces = board.pieces();

	int			white  = 0;
	int			black  = 0;

	white += BitUtils::popCount(pieces[PieceType::WPawn]) * PieceValues::PAWN;
	white += BitUtils::popCount(pieces[PieceType::WKnight]) * PieceValues::KNIGHT;
	white += BitUtils::popCount(pieces[PieceType::WBishop]) * PieceValues::BISHOP;
	white += BitUtils::popCount(pieces[PieceType::WRook]) * PieceValues::ROOK;
	white += BitUtils::popCount(pieces[PieceType::WQueen]) * PieceValues::QUEEN;

	black += BitUtils::popCount(pieces[PieceType::BPawn]) * PieceValues::PAWN;
	black += BitUtils::popCount(pieces[PieceType::BKnight]) * PieceValues::KNIGHT;
	black += BitUtils::popCount(pieces[PieceType::BBishop]) * PieceValues::BISHOP;
	black += BitUtils::popCount(pieces[PieceType::BRook]) * PieceValues::ROOK;
	black += BitUtils::popCount(pieces[PieceType::BQueen]) * PieceValues::QUEEN;

	return white - black;
}


int Evaluation::evaluatePieceSquareTables(const Chessboard &board)
{
	const auto &pieces = board.pieces();

	int			score  = 0;

	// White pieces (tables are from white's perspective)
	score += scorePieceSquare(pieces[PieceType::WPawn], PST_PAWN, true);
	score += scorePieceSquare(pieces[PieceType::WKnight], PST_KNIGHT, true);
	score += scorePieceSquare(pieces[PieceType::WBishop], PST_BISHOP, true);
	score += scorePieceSquare(pieces[PieceType::WRook], PST_ROOK, true);
	score += scorePieceSquare(pieces[PieceType::WQueen], PST_QUEEN, true);
	score += scorePieceSquare(pieces[PieceType::WKing], PST_KING_MIDDLEGAME, true);

	// Black pieces (mirror the square index)
	score -= scorePieceSquare(pieces[PieceType::BPawn], PST_PAWN, false);
	score -= scorePieceSquare(pieces[PieceType::BKnight], PST_KNIGHT, false);
	score -= scorePieceSquare(pieces[PieceType::BBishop], PST_BISHOP, false);
	score -= scorePieceSquare(pieces[PieceType::BRook], PST_ROOK, false);
	score -= scorePieceSquare(pieces[PieceType::BQueen], PST_QUEEN, false);
	score -= scorePieceSquare(pieces[PieceType::BKing], PST_KING_MIDDLEGAME, false);

	return score;
}


int Evaluation::evaluatePawnStructure(const Chessboard &board)
{
	// TODO: Evaluate doubled, isolated, and passed pawns
	return 0;
}


int Evaluation::evaluateKingSafety(const Chessboard &board)
{
	// TODO: Evaluate king pawn shield, open files near king,...
	return 0;
}


int Evaluation::evaluateMobility(const Chessboard &board)
{
	// Evaluate piece mobility
	return 0;
}


int Evaluation::scorePieceSquare(U64 bitboard, const int table[64], bool isWhite)
{
	int score = 0;

	while (bitboard)
	{
		int sq = BitUtils::lsb(bitboard);
		score += table[isWhite ? sq : mirrorSquare(sq)];
		bitboard &= bitboard - 1; // clear LSB
	}

	return score;
}
