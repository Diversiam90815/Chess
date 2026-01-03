/*
  ==============================================================================
	Module:         GameEngine
	Description:    Engine of the chess game
  ==============================================================================
*/


#pragma once

#include <optional>

#include "Execution/MoveExecution.h"
#include "Generation/MoveGeneration.h"
#include "CPUPlayer.h"
#include "IObservable.h"
#include "Parameters.h"
#include "Player.h"


/// <summary>
/// Main engine for managing and controlling the state and logic of a chess game.
/// </summary>
class GameEngine : public IGameObservable
{
public:
	GameEngine();
	~GameEngine() = default;

	void					   init();
	void					   reset();
	void					   startGame();
	void					   resetGame();

	// Move operations
	void					   makeMove(Move move, bool fromRemote = false);
	void					   undoMove();

	// Move generation
	void					   generateMoves(MoveList &moves);
	bool					   isLegalMove(Move move);
	
	// Game state
	bool					   isInCheck() const;
	bool					   isCheckmate();
	bool					   isStalemate();
	bool					   isDraw() const;

	// Player management
	void					   switchTurns();
	Side					   getCurrentSide() const { return mChessBoard.getCurrentSide(); }
	PlayerColor				   getCurrentPlayer() const;
	void					   changeCurrentPlayer(PlayerColor player) override;

	void					   setLocalPlayer(PlayerColor player);
	PlayerColor				   getLocalPlayer() const;

	// End game
	EndGameState			   checkForEndGameConditions();
	void					   endGame(EndGameState state, PlayerColor player = PlayerColor::None) override;
	std::optional<PlayerColor> getWinner() const;

	// CPU Player
	void					   setCPUConfiguration(const CPUConfiguration &config);
	CPUConfiguration		   getCPUConfiguration() const;
	bool					   isCPUPlayer(PlayerColor player) const;
	void					   requestCPUMoveAsync();

	// Accessors
	const Chessboard		  &getBoard() const { return mChessBoard; }
	Chessboard				  &getBoard() { return mChessBoard; }


	//// PieceType				   getCurrentPieceTypeAtPosition(const Position position);

	//std::vector<PossibleMove>  getPossibleMoveForPosition();

	//// bool					   getBoardState(BoardStateArray boardState);

	//bool					   checkForValidMoves(const PossibleMove &move);
	//bool					   checkForPawnPromotionMove(const PossibleMove &move);





	//bool					   calculateAllMovesForPlayer();

	//bool					   initiateMove(const Position &startPosition);




private:
	//bool					  mMovesGeneratedForCurrentTurn = false;

	Player					  mWhitePlayer;
	Player					  mBlackPlayer;
	PlayerColor				  mCurrentPlayer = PlayerColor::None;

	//std::vector<PossibleMove> mAllMovesForPosition;

	Chessboard				  mChessBoard;
	MoveGeneration			  mMoveGeneration;
	MoveExecution			  mMoveExecution;
	CPUPlayer				  mCPUPlayer;

	std::mutex				  mMutex;

	friend class GameManager;
};
