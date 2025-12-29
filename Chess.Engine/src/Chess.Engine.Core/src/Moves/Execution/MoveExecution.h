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
#include "Validation/MoveValidation.h"
#include "Notation/MoveNotation.h"
#include "Move.h"
#include "IObservable.h"
#include "Logging.h"


/**
 * @brief	Manages execution of moves on a chessboard, including special cases
 *			(castling, en passant, promotion) and history maintenance.
 */
class MoveExecution : public IMoveObservable
{
public:
	MoveExecution(std::shared_ptr<ChessBoard> board, std::shared_ptr<MoveValidation> validation);
	~MoveExecution() = default;

	/**
	 * @brief	Execute a possible move (validity assumed pre-checked) and notify observers.
	 * @param	executedMove -> Move descriptor (may be enriched).
	 * @param	fromRemote -> True if originated from remote peer (affects notifications).
	 * @return	Concrete Move committed to history.
	 */
	Move		executeMove(PossibleMove &executedMove, bool fromRemote = false) override;

	/**
	 * @brief	Execute castling (updates king & rook positions).
	 * @return	true if successful.
	 */
	bool		executeCastlingMove(PossibleMove &move);

	/**
	 * @brief	Execute an en passant capture if legal.
	 */
	bool		executeEnPassantMove(PossibleMove &move, PlayerColor player);

	/**
	 * @brief	Promote a pawn to another piece type.
	 */
	bool		executePawnPromotion(const PossibleMove &move, PlayerColor player);

	/**
	 * @brief	Gets the last executed move in history.
	 * @return	const pointer to the move, or nullptr is none
	 */
	const Move *getLastMove() const;

	/**
	 * @brief	Add move to internal history (observer callback contract).
	 */
	void		addMoveToHistory(Move &move) override;

	/**
	 * @brief	Clear complete move history.
	 */
	void		clearMoveHistory() override;

	/**
	 * @brief	Remove last move (used for undo).
	 */
	void		removeLastMove();


private:
	std::shared_ptr<ChessBoard>		mChessBoard;
	std::shared_ptr<MoveValidation> mValidation;
	std::shared_ptr<MoveNotation>	mMoveNotation;
	std::set<Move>					mMoveHistory;
	std::mutex						mExecutionMutex;
};
