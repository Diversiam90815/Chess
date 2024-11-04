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
	MovementManager();

	~MovementManager() = default;


	std::vector<PossibleMove> getMovesForPosition(Position &position);

	bool					  calculateAllLegalBasicMoves(PlayerColor playerColor);

	Move					  executeMove(PossibleMove &executedMove, PieceType pawnPromotion = PieceType::DefaultType);


private:
	bool													validateMove(Move &move, PlayerColor playerColor);

	bool													isKingInCheck(Position &ourKing, PlayerColor playerColor);

	bool													isCheckmate(PlayerColor player);

	bool													isStalemate(PlayerColor player);

	bool													wouldKingBeInCheckAfterMove(Move &move, PlayerColor playerColor);

	bool													isSquareAttacked(const Position &square, PlayerColor attackerColor);


	bool													executeCastlingMove(PossibleMove &move);

	std::vector<PossibleMove>								generateCastlingMoves(const Position &kingPosition, PlayerColor player);

	bool													canCastle(const Position &kingposition, PlayerColor player, bool kingside);


	bool													executeEnPassantMove(PossibleMove &move, PlayerColor player);

	PossibleMove											generateEnPassantMove(const Position &position, PlayerColor player);

	bool													canEnPassant(const Position &position, PlayerColor player);


	bool													executePawnPromotion(const PossibleMove &move, PieceType promotedType);


	const Move											   *getLastMove();

	void													addMoveToHistory(Move &move);


	std::unordered_map<Position, std::vector<PossibleMove>> mAllLegalMovesForCurrentRound;

	std::set<Move>											mMoveHistory;

	std::unique_ptr<ChessBoard>								mChessBoard;


	friend class GameManager;
};
