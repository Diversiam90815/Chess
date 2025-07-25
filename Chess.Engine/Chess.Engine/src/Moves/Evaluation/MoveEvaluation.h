/*
  ==============================================================================
	Module:         MoveEvaluation
	Description:    Evaluation of moves
  ==============================================================================
*/

#pragma once

#include <array>

#include "Move.h"
#include "ChessBoard.h"


class MoveEvaluation
{
public:
	MoveEvaluation(std::shared_ptr<ChessBoard> chessboard);
	~MoveEvaluation() = default;

	int getBasicEvaluation(const PossibleMove &move);

	int getMediumEvaluation(const PossibleMove &move, PlayerColor player);

	int getAdvancedEvaluation(const PossibleMove &move, PlayerColor player);

private:
	static constexpr int							   getPieceValue(PieceType piece);

	int												   getPositionValue(PieceType piece, const Position &pos, PlayerColor player);

	int												   evaluateMaterialGain(const PossibleMove &move);

	int												   evaluatePositionalGain(const PossibleMove &move, PlayerColor player);


	std::shared_ptr<ChessBoard>						   mBoard;

	static constexpr int							   PAWN_VALUE	= 100;
	static constexpr int							   KNIGHT_VALUE = 320;
	static constexpr int							   BISHOP_VALUE = 330;
	static constexpr int							   ROOK_VALUE	= 500;
	static constexpr int							   QUEEN_VALUE	= 900;

	// Position value tables (from white's perspective, flip for black)
	static constexpr std::array<std::array<int, 8>, 8> PAWN_TABLE	= {{{0, 0, 0, 0, 0, 0, 0, 0},
																		{50, 50, 50, 50, 50, 50, 50, 50},
																		{10, 10, 20, 30, 30, 20, 10, 10},
																		{5, 5, 10, 25, 25, 10, 5, 5},
																		{0, 0, 0, 20, 20, 0, 0, 0},
																		{5, -5, -10, 0, 0, -10, -5, 5},
																		{5, 10, 10, -20, -20, 10, 10, 5},
																		{0, 0, 0, 0, 0, 0, 0, 0}}};

	static constexpr std::array<std::array<int, 8>, 8> KNIGHT_TABLE = {{{-50, -40, -30, -30, -30, -30, -40, -50},
																		{-40, -20, 0, 0, 0, 0, -20, -40},
																		{-30, 0, 10, 15, 15, 10, 0, -30},
																		{-30, 5, 15, 20, 20, 15, 5, -30},
																		{-30, 0, 15, 20, 20, 15, 0, -30},
																		{-30, 5, 10, 15, 15, 10, 5, -30},
																		{-40, -20, 0, 5, 5, 0, -20, -40},
																		{-50, -40, -30, -30, -30, -30, -40, -50}}};

	static constexpr std::array<std::array<int, 8>, 8> BISHOP_TABLE = {{{-20, -10, -10, -10, -10, -10, -10, -20},
																		{-10, 0, 0, 0, 0, 0, 0, -10},
																		{-10, 0, 5, 10, 10, 5, 0, -10},
																		{-10, 5, 5, 10, 10, 5, 5, -10},
																		{-10, 0, 10, 10, 10, 10, 0, -10},
																		{-10, 10, 10, 10, 10, 10, 10, -10},
																		{-10, 5, 0, 0, 0, 0, 5, -10},
																		{-20, -10, -10, -10, -10, -10, -10, -20}}};

	static constexpr std::array<std::array<int, 8>, 8> ROOK_TABLE	= {{{0, 0, 0, 0, 0, 0, 0, 0},
																		{5, 10, 10, 10, 10, 10, 10, 5},
																		{-5, 0, 0, 0, 0, 0, 0, -5},
																		{-5, 0, 0, 0, 0, 0, 0, -5},
																		{-5, 0, 0, 0, 0, 0, 0, -5},
																		{-5, 0, 0, 0, 0, 0, 0, -5},
																		{-5, 0, 0, 0, 0, 0, 0, -5},
																		{0, 0, 0, 5, 5, 0, 0, 0}}};

	static constexpr std::array<std::array<int, 8>, 8> QUEEN_TABLE	= {{{-20, -10, -10, -5, -5, -10, -10, -20},
																		{-10, 0, 0, 0, 0, 0, 0, -10},
																		{-10, 0, 5, 5, 5, 5, 0, -10},
																		{-5, 0, 5, 5, 5, 5, 0, -5},
																		{0, 0, 5, 5, 5, 5, 0, -5},
																		{-10, 5, 5, 5, 5, 5, 0, -10},
																		{-10, 0, 5, 0, 0, 0, 0, -10},
																		{-20, -10, -10, -5, -5, -10, -10, -20}}};

	static constexpr std::array<std::array<int, 8>, 8> KING_TABLE	= {{{-30, -40, -40, -50, -50, -40, -40, -30},
																		{-30, -40, -40, -50, -50, -40, -40, -30},
																		{-30, -40, -40, -50, -50, -40, -40, -30},
																		{-30, -40, -40, -50, -50, -40, -40, -30},
																		{-20, -30, -30, -40, -40, -30, -30, -20},
																		{-10, -20, -20, -20, -20, -20, -20, -10},
																		{20, 20, 0, 0, 0, 0, 20, 20},
																		{20, 30, 10, 0, 0, 10, 30, 20}}};
};
