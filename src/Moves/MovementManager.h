/*
  ==============================================================================

	Class:          MovementManager

	Description:    Manages and executes valid moves in chess

  ==============================================================================
*/


#pragma once

#include "ChessBoard.h"
#include "Move.h"
#include <set>
#include <unordered_map>


class MovementManager
{
public:
	MovementManager(ChessBoard &board);

	std::unordered_map<Position, std::vector<PossibleMove>> getAllPossibleMoves(PieceColor playerColor);

private:
	bool		validateMove(Move &move, PieceColor playerColor);

	bool		isKingInCheck(Position &ourKing, PieceColor playerColor);

	bool		wouldKingBeInCheckAfterMove(Move &move, PieceColor playerColor);

	bool		isSquareAttacked(const Position &square, PieceColor attackerColor);


	ChessBoard &board;
};
