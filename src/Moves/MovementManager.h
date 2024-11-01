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

	std::vector<PossibleMove> getMovesForPosition(Position &position);

	bool					  calculateAllLegalBasicMoves(PieceColor playerColor);

	bool					  executeMove(const Move &move);

private:
	bool													validateMove(Move &move, PieceColor playerColor);

	bool													isKingInCheck(Position &ourKing, PieceColor playerColor);

	bool													wouldKingBeInCheckAfterMove(Move &move, PieceColor playerColor);

	bool													isSquareAttacked(const Position &square, PieceColor attackerColor);


	std::vector<PossibleMove>								generateCastlingMoves(const Position &kingPosition, PieceColor player);

	bool													canCastleKingside(const Position &kingsPosition, PieceColor player);

	bool													canCastleQueenside(const Position &kingsPosition, PieceColor player);


	const Move											   *getLastMove();

	void													addMoveToHistory(Move &move);


	ChessBoard											   &board;

	std::unordered_map<Position, std::vector<PossibleMove>> mAllLegalMovesForCurrentRound;

	std::set<Move>											mMoveHistory;
};
