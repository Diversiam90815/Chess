/*
  ==============================================================================
	Module:			MoveValidation
	Description:    Manages the validation of moves in chess
  ==============================================================================
*/

#pragma once

#include <memory>

#include "ChessBoard.h"


#define VALIDATION_DEBUG false

/**
 * @brief Provides methods to validate chess moves and evaluate king safety,
 *        check, checkmate, and stalemate conditions.
 *
 * Thread-safety: Not inherently thread-safe; external synchronization required
 * if the underlying ChessBoard is shared across threads.
 */
class MoveValidation
{
public:
	MoveValidation() = default;
	MoveValidation(std::shared_ptr<ChessBoard> board);

	~MoveValidation();

	/**
	 * @brief (Re)initialize with a chessboard (must be called before use if
	 * default constructed).
	 */
	void init(std::shared_ptr<ChessBoard> board);

	/**
	 * @brief Validate a concrete move considering piece rules and king safety.
	 * @param move Move to validate (may be updated with flags such as capture / special).
	 * @param playerColor Color of the player executing the move.
	 * @return true if legal.
	 */
	bool validateMove(Move &move, PlayerColor playerColor);

	/**
	 * @brief Determine if the specified king position is currently attacked.
	 * @param ourKing Current king position.
	 * @param playerColor Color of the king owner.
	 * @return true if in check.
	 */
	bool isKingInCheck(const Position &ourKing, PlayerColor playerColor);

	/**
	 * @brief Test for checkmate against the given player.
	 */
	bool isCheckmate(PlayerColor player);

	/**
	 * @brief Test for stalemate against the given player.
	 */
	bool isStalemate(PlayerColor player);

	/**
	 * @brief Simulate a move and check if it would leave the king in check.
	 * @return true if king would be (or remain) in check after the move.
	 */
	bool wouldKingBeInCheckAfterMove(Move &move, PlayerColor playerColor);


private:
	/**
	 * @brief Determine if a square is attacked (uses internal board).
	 */
	bool						isSquareAttacked(const Position &square, PlayerColor attackerColor);

	/**
	 * @brief Determine if a square is attacked using a provided temporary board.
	 */
	bool						isSquareAttacked(const Position &square, PlayerColor attackerColor, ChessBoard &chessboard);

	std::shared_ptr<ChessBoard> mChessBoard;
};
