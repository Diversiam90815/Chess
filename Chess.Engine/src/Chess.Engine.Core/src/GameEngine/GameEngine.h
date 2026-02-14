/*
  ==============================================================================
	Module:         GameEngine
	Description:    Engine of the chess game
  ==============================================================================
*/

#pragma once

#include "Execution/MoveExecution.h"
#include "Generation/MoveGeneration.h"
#include "Validation/MoveValidation.h"
#include "IObservable.h"
#include "Parameters.h"
#include "Player.h"

/**
 * @brief	Core chess engine orchestrating board state, move generation,
 *			validation, execution, and game flow.
 */
class GameEngine
{
public:
	GameEngine();
	~GameEngine() = default;

	// Lifecycle
	void								 init();
	void								 startGame();
	void								 resetGame();

	//=========================================================================
	// Move Operations
	//=========================================================================

	/**
	 * @brief	Execute a move. Validates, applies, and notifies observers.
	 * @param	move	Move to execute
	 * @param	fromRemote	Flag indicating that the move came from the remote endpoint in a
							Multiplayer setting to avoid infite loop.
	 * @return	MoveExecutionResult		Success indicator and notation on success
	 */
	MoveExecutionResult					 makeMove(Move move);

	/**
	 * @brief	Undo the last move. Notifies observers.
	 * @return	true if a move was undone.
	 */
	bool								 undoMove();

	//=========================================================================
	// Move Generation & Validation
	//=========================================================================

	/**
	 * @brief	Generate all legal moves for current side.
	 */
	void								 generateLegalMoves(MoveList &moves);

	/**
	 * @brief	Check if a move is legal.
	 */
	bool								 isMoveLegal(Move move);

	/**
	 * @brief	Get legal moves from a specific square (for UI).
	 */
	void								 getMovesFromSquare(Square from, MoveList &moves);

	//=========================================================================
	//	Game State Queries
	//=========================================================================

	bool								 isInCheck() const;
	bool								 isCheckmate();
	bool								 isStalemate();
	bool								 isDraw() const;
	EndGameState						 checkForEndGameConditions();

	//=========================================================================
	//	Player Management
	//=========================================================================

	void								 switchTurns();
	Side								 getCurrentSide() const { return mChessBoard.getCurrentSide(); }
	void								 changeCurrentPlayer(Side player);
	void								 setLocalPlayer(Side player);
	Side								 getLocalPlayer() const;

	//=========================================================================
	//	End Game
	//=========================================================================

	Side								 getWinner() const;

	//=========================================================================
	//	CPU Player
	//=========================================================================

	uint64_t							 getHash();

	//=========================================================================
	//	Accessors
	//=========================================================================

	const Chessboard					&getBoard() const { return mChessBoard; }
	Chessboard							&getBoard() { return mChessBoard; }

	const std::vector<MoveHistoryEntry> &getMoveHistory() const;
	std::string							 getMoveNotation(Move move) const;


private:
	// Core components (order matters for initialization!)
	Chessboard	   mChessBoard;
	MoveGeneration mMoveGeneration;
	MoveExecution  mMoveExecution;
	MoveValidation mMoveValidation;

	// Players
	Player		   mWhitePlayer;
	Player		   mBlackPlayer;
	Side		   mCurrentPlayer = Side::None;

	// Thread safety
	std::mutex	   mMoveMutex;

	friend class GameManager;
};
