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


#define GENERATION_DEBUG false


class MoveGeneration
{
public:
	MoveGeneration(Chessboard &board);
	~MoveGeneration() = default;

	std::vector<PossibleMove> getMovesForPosition(const Position &position);

	bool					  calculateAllLegalBasicMoves(PlayerColor playerColor);


private:
	bool													isSquareAttacked(int square, Side side) const;


	std::vector<PossibleMove>								generateCastlingMoves(const Position &kingPosition, PlayerColor player);

	bool													canCastle(const Position &kingposition, PlayerColor player, bool kingside);

	PossibleMove											generateEnPassantMove(const Position &position, PlayerColor player);

	bool													canEnPassant(const Position &position, PlayerColor player);

	void													loadMoveToMap(Position pos, std::vector<PossibleMove> moves);


	Chessboard											   &mChessBoard;

	std::unordered_map<Position, std::vector<PossibleMove>> mAllLegalMovesForCurrentRound;
	std::mutex												mMoveMutex;
};
