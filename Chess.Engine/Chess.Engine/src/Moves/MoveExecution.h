/*
  ==============================================================================
	Module:			MoveExecution
	Description:    Manages the execution of moves in chess
  ==============================================================================
*/

#pragma once

#include <memory>
#include <set>

#include "MoveValidation.h"

#include "ChessBoard.h"
#include "MoveNotationHelper.h"
#include "Move.h"
#include "IObservable.h"


class MoveExecution : public IMoveObservable
{
public:
	MoveExecution(std::shared_ptr<ChessBoard> board, std::shared_ptr<MoveValidation> validation);
	~MoveExecution() = default;

	Move		executeMove(PossibleMove &executedMove, bool fromRemote = false) override;

	bool		executeCastlingMove(PossibleMove &move);

	bool		executeEnPassantMove(PossibleMove &move, PlayerColor player);

	bool		executePawnPromotion(const PossibleMove &move, PlayerColor player);

	const Move *getLastMove();

	void		addMoveToHistory(Move &move) override;

	void		clearMoveHistory() override;

	void		removeLastMove();


private:
	std::shared_ptr<ChessBoard>			mChessBoard;

	std::shared_ptr<MoveValidation>		mValidation;

	std::shared_ptr<MoveNotationHelper> mMoveNotation;

	std::set<Move>						mMoveHistory;
};
