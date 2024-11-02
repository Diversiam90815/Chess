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

/*
Handling moves still need to be implemented:
	- Stalemate
	- Checkmate
	- Check
	- PawnPromotion
*/


class MovementManager
{
public:
	MovementManager();

	~MovementManager() = default;


	std::vector<PossibleMove> getMovesForPosition(Position &position);

	bool					  calculateAllLegalBasicMoves(PieceColor playerColor);

	Move					  executeMove(PossibleMove &executedMove);


private:
	bool													validateMove(Move &move, PieceColor playerColor);

	bool													isKingInCheck(Position &ourKing, PieceColor playerColor);

	bool													wouldKingBeInCheckAfterMove(Move &move, PieceColor playerColor);

	bool													isSquareAttacked(const Position &square, PieceColor attackerColor);


	bool													executeCastlingMove(PossibleMove &move);

	std::vector<PossibleMove>								generateCastlingMoves(const Position &kingPosition, PieceColor player);

	bool													canCastle(const Position &kingposition, PieceColor player, bool kingside);


	bool													executeEnPassantMove(PossibleMove &move, PieceColor player);

	PossibleMove											generateEnPassantMove(const Position &position, PieceColor player);

	bool													canEnPassant(const Position &position, PieceColor player);


	const Move											   *getLastMove();

	void													addMoveToHistory(Move &move);


	std::unordered_map<Position, std::vector<PossibleMove>> mAllLegalMovesForCurrentRound;

	std::set<Move>											mMoveHistory;

	std::unique_ptr<ChessBoard>								mChessBoard;
};
