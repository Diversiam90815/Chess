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

#include "ChessBoard.h"
#include "Move.h"


class MoveGeneration
{
public:
	MoveGeneration(std::shared_ptr<ChessBoard> board);
	~MoveGeneration();



	std::vector<PossibleMove> getMovesForPosition(Position &position);

	bool					  calculateAllLegalBasicMoves(PlayerColor playerColor);


private:
	std::vector<PossibleMove>								generateCastlingMoves(const Position &kingPosition, PlayerColor player);

	bool													canCastle(const Position &kingposition, PlayerColor player, bool kingside);


	PossibleMove											generateEnPassantMove(const Position &position, PlayerColor player);

	bool													canEnPassant(const Position &position, PlayerColor player);

	void													loadMoveToMap(Position pos, std::vector<PossibleMove> moves);


	std::shared_ptr<ChessBoard>								mChessBoard;

	std::unordered_map<Position, std::vector<PossibleMove>> mAllLegalMovesForCurrentRound;

	std::mutex												mMoveMutex;
};
