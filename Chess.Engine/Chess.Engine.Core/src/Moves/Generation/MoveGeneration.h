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

#include "ChessBoard.h"
#include "Move.h"
#include "Validation/MoveValidation.h"
#include "Execution/MoveExecution.h"


#define GENERATION_DEBUG false


class MoveGeneration
{
public:
	MoveGeneration(std::shared_ptr<ChessBoard> board, std::shared_ptr<MoveValidation> validation, std::shared_ptr<MoveExecution> execution);
	~MoveGeneration();

	std::vector<PossibleMove> getMovesForPosition(const Position &position);

	bool					  calculateAllLegalBasicMoves(PlayerColor playerColor);

private:
	std::vector<PossibleMove>								generateCastlingMoves(const Position &kingPosition, PlayerColor player);

	bool													canCastle(const Position &kingposition, PlayerColor player, bool kingside);

	PossibleMove											generateEnPassantMove(const Position &position, PlayerColor player);

	bool													canEnPassant(const Position &position, PlayerColor player);

	void													loadMoveToMap(Position pos, std::vector<PossibleMove> moves);


	std::shared_ptr<ChessBoard>								mChessBoard;

	std::shared_ptr<MoveValidation>							mValidation;

	std::shared_ptr<MoveExecution>							mExecution;

	std::unordered_map<Position, std::vector<PossibleMove>> mAllLegalMovesForCurrentRound;

	std::mutex												mMoveMutex;
};
