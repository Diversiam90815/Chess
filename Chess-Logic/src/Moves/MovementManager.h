/*
  ==============================================================================

	Class:          MovementManager

	Description:    Manages and executes valid moves in chess

  ==============================================================================
*/

#pragma once

#include <set>
#include <unordered_map>
#include <mutex>

#include "ChessLogicAPIDefines.h"
#include "ChessBoard.h"
#include "Move.h"
#include "MoveNotationHelper.h"
#include "Pawn.h"
#include "Knight.h"
#include "Bishop.h"
#include "Rook.h"
#include "Queen.h"
#include "King.h"


class MovementManager
{
public:
	MovementManager();

	~MovementManager() = default;

	void					  init();

	std::vector<PossibleMove> getMovesForPosition(Position &position);

	bool					  calculateAllLegalBasicMoves(PlayerColor playerColor);

	Move					  executeMove(PossibleMove &executedMove);

	void					  removeLastMove();

	void					  setDelegate(PFN_CALLBACK pDelegate);

private:
	void													loadMoveToMap(Position pos, std::vector<PossibleMove> moves);

	bool													validateMove(Move &move, PlayerColor playerColor);

	bool													isKingInCheck(Position &ourKing, PlayerColor playerColor);

	bool													isCheckmate(PlayerColor player);

	bool													isStalemate(PlayerColor player);

	bool													wouldKingBeInCheckAfterMove(Move &move, PlayerColor playerColor);

	bool													isSquareAttacked(const Position &square, PlayerColor attackerColor);
	bool													isSquareAttacked(const Position &square, PlayerColor attackerColor, ChessBoard &chessboard);


	bool													executeCastlingMove(PossibleMove &move);

	std::vector<PossibleMove>								generateCastlingMoves(const Position &kingPosition, PlayerColor player);

	bool													canCastle(const Position &kingposition, PlayerColor player, bool kingside);


	bool													executeEnPassantMove(PossibleMove &move, PlayerColor player);

	PossibleMove											generateEnPassantMove(const Position &position, PlayerColor player);

	bool													canEnPassant(const Position &position, PlayerColor player);


	bool													executePawnPromotion(const PossibleMove &move, PlayerColor player);


	const Move											   *getLastMove();

	void													addMoveToHistory(Move &move);


	std::unordered_map<Position, std::vector<PossibleMove>> mAllLegalMovesForCurrentRound;

	std::set<Move>											mMoveHistory;

	std::unique_ptr<ChessBoard>								mChessBoard;

	std::unique_ptr<MoveNotationHelper>						mMoveNotation;

	std::mutex												mMoveMutex;

	PFN_CALLBACK											mDelegate = nullptr;

	friend class GameManager;
};
