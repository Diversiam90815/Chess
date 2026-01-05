/*
  ==============================================================================
	Module:         GameController
	Description:    IGameController implementation
  ==============================================================================
*/

#include <memory>

#include "IGameController.h"
#include "GameEngine.h"
#include "CPUPlayer.h"


class GameController : public IGameController
{
public:
	GameController();
	~GameController() override = default;

	//=========================================================================
	// Game Lifecycle
	//=========================================================================

	bool								 initializeGame(GameConfiguration config) override;
	void								 resetGame() override;

	//=========================================================================
	// Move Operations
	//=========================================================================

	void								 getLegalMovesFromSquare(Square sq, MoveList &moves) override;
	bool								 executeMove(Move move, bool fromRemote) override;
	bool								 undoLastMove() override;

	//=========================================================================
	// Queries
	//=========================================================================

	bool								 isPromotionMove(Square from, Square to) const override;
	Move								 findMove(Square from, Square to, PieceTypes promotion = PieceTypes::None) const override;
	EndGameState						 checkEndGame() override;

	//=========================================================================
	// Turn Management
	//=========================================================================

	Side								 getCurrentSide() const override;
	bool								 isLocalPlayerTurn() const override;
	void								 switchTurns() override;

	//=========================================================================
	// CPU
	//=========================================================================

	bool								 isCPUTurn() const override;
	void								 requestCPUMoveAsync() override;
	void								 cancelCPUCalculation();

	//=========================================================================
	// Accessors (for UI board state queries)
	//=========================================================================

	const Chessboard					&getBoard() const { return mEngine.getBoard(); }
	const std::vector<MoveHistoryEntry> &getMoveHistory() const { return mEngine.getMoveHistory(); }
	const MoveList						&getCachedLegalMoves() { return mCachedLegalMoves; }

	void								 setCPUMoveCallback(std::function<void(Move)> callback);

private:
	GameEngine				  mEngine;
	CPUPlayer				  mCPUPlayer;

	Side					  mLocalPlayer{Side::White};
	GameConfiguration		  mConfig{};

	// Cached legal moves for current position (for findMove)
	mutable MoveList		  mCachedLegalMoves;
	mutable bool			  mCacheValid{false};

	std::function<void(Move)> mOnCPUMove;

	void					  invalidateCache() { mCacheValid = false; }
	void					  ensureCacheValid() const;
};
