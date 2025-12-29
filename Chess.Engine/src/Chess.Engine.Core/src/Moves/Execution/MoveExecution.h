/*
  ==============================================================================
	Module:			MoveExecution
	Description:    Manages the execution of moves in chess
  ==============================================================================
*/

#pragma once

#include <memory>
#include <set>
#include <mutex>

#include "ChessBoard.h"
#include "Notation/MoveNotation.h"
#include "Move.h"
#include "IObservable.h"
#include "Logging.h"


class MoveExecution : public IMoveObservable
{
public:
	MoveExecution(Chessboard &board);
	~MoveExecution() = default;

	Move		executeMove(PossibleMove &executedMove, bool fromRemote = false) override;

	bool		executeCastlingMove(PossibleMove &move);

	bool		executeEnPassantMove(PossibleMove &move, PlayerColor player);

	bool		executePawnPromotion(const PossibleMove &move, PlayerColor player);

	const Move *getLastMove() const;

	void		addMoveToHistory(Move &move) override;

	void		clearMoveHistory() override;

	void		removeLastMove();


private:
	Chessboard	  &mChessBoard;
	MoveNotation   mMoveNotation;
	std::set<Move> mMoveHistory;
	std::mutex	   mExecutionMutex;
};
