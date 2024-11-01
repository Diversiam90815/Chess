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
	MovementManager(std::shared_ptr<ChessBoard> board);

	~MovementManager() = default;


	std::vector<PossibleMove> getMovesForPosition(Position &position);

	bool					  calculateAllLegalBasicMoves(PieceColor playerColor);

	bool					  executeMove(Move &move);


private:
	bool													validateMove(Move &move, PieceColor playerColor);

	bool													isKingInCheck(Position &ourKing, PieceColor playerColor);

	bool													wouldKingBeInCheckAfterMove(Move &move, PieceColor playerColor);

	bool													isSquareAttacked(const Position &square, PieceColor attackerColor);


	std::vector<PossibleMove>								generateCastlingMoves(const Position &kingPosition, PieceColor player);

	bool													canCastle(const Position &kingposition, PieceColor player, bool kingside);


	const Move											   *getLastMove();

	void													addMoveToHistory(Move &move);


	std::shared_ptr<ChessBoard>								board;

	std::unordered_map<Position, std::vector<PossibleMove>> mAllLegalMovesForCurrentRound;

	std::set<Move>											mMoveHistory;
};
