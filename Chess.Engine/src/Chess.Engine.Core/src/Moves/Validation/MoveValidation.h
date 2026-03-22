/*
  ==============================================================================
	Module:         MoveValidation
	Description:    Move legality validation
  ==============================================================================
*/

#include "Chessboard.h"
#include "Move.h"
#include "Generation/MoveGeneration.h"
#include "Execution/MoveExecution.h"


class MoveValidation
{
public:
	MoveValidation(Chessboard &board, MoveGeneration &generation, MoveExecution &execution);
	~MoveValidation() = default;

	/**
	 * @brief Check if the side to move is in check.
	 */
	[[nodiscard]] bool	 isInCheck() const;

	/**
	 * @brief Check if a specific side's king is attacked.
	 */
	[[nodiscard]] bool	 isKingAttacked(Side side) const;

	/**
	 * @brief Check if a move is legal (doesn't leave own king in check).
	 */
	[[nodiscard]] bool	 isMoveLegal(Move move);

	/**
	 * @brief Check if current position is checkmate.
	 */
	[[nodiscard]] bool	 isCheckmate();

	/**
	 * @brief Check if current position is stalemate.
	 */
	[[nodiscard]] bool	 isStalemate();

	/**
	 * @brief Check for draw conditions (50-move, insufficient material).
	 */
	[[nodiscard]] bool	 isDraw() const;

	/**
	 * @brief Generate all legal moves for current side.
	 */
	void				 generateLegalMoves(MoveList &legalMoves);

	/**
	 * @brief Count legal moves (optimization: can early-exit for checkmate/stalemate).
	 */
	[[nodiscard]] size_t countLegalMoves();


private:
	[[nodiscard]] Square getKingSquare(Side side) const;
	[[nodiscard]] bool	 hasInsufficientMaterial() const;

	Chessboard			&mBoard;
	MoveGeneration		&mGeneration;
	MoveExecution		&mExecution;
};