/*
  ==============================================================================
	Module:         MoveGeneration
	Description:    Manages the generation of moves
  ==============================================================================
*/

#pragma once

#include <vector>
#include <unordered_map>
#include <future>
#include <memory>

#include "Chessboard.h"
#include "Logging.h"
#include "Move.h"
#include "Execution/MoveExecution.h"
#include "BitboardTypes.h"


#define GENERATION_DEBUG false


class MoveGeneration
{
public:
	explicit MoveGeneration(Chessboard &board);
	~MoveGeneration() = default;

	bool isSquareAttacked(Square square, Side attacker) const;
	void generateAllMoves(MoveList &moves);

private:
	void		generateCastlingMoves(MoveList &moves, Side side);
	void		generatePawnMoves(MoveList &moves, Side side);
	void		generateKnightMoves(MoveList &moves, Side side);
	void		generateRookMoves(MoveList &moves, Side side);
	void		generateBishopMoves(MoveList &moves, Side side);
	void		generateQueenMoves(MoveList &moves, Side side);
	void		generateKingMoves(MoveList &moves, Side side);

	void		addSlidingMoves(MoveList &moves, Square from, U64 attacks, U64 enemyOcc);

	Chessboard &mChessBoard;
};
