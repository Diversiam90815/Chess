/*
  ==============================================================================
	Module:			MoveExecution
	Description:    Manages the execution of moves in chess
  ==============================================================================
*/

#pragma once

#include <memory>
#include <set>

#include "ChessBoard.h"
#include "MoveNotationHelper.h"
#include "Move.h"


class MoveExecution
{
public:
	MoveExecution(std::shared_ptr<ChessBoard> board);
	~MoveExecution();

	Move executeMove(PossibleMove &executedMove);

	bool executeCastlingMove(PossibleMove &move);

	bool executeEnPassantMove(PossibleMove &move, PlayerColor player);

	bool executePawnPromotion(const PossibleMove &move, PlayerColor player);


private:
	const Move						   *getLastMove();

	void								addMoveToHistory(Move &move);

	void								removeLastMove();

	std::shared_ptr<ChessBoard>			mChessBoard;

	std::shared_ptr<MoveNotationHelper> mMoveNotation;
	std::set<Move>						mMoveHistory;
};
