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
#include "Evaluation/MoveEvaluation.h"
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

	void					   executeMove(PossibleMove &tmpMove, bool fromRemote = false);
	void					   undoMove();

	// PieceType				   getCurrentPieceTypeAtPosition(const Position position);

	std::vector<PossibleMove>  getPossibleMoveForPosition();

	// bool					   getBoardState(BoardStateArray boardState);

	bool					   checkForValidMoves(const PossibleMove &move);
	bool					   checkForPawnPromotionMove(const PossibleMove &move);

	std::optional<PlayerColor> getWinner() const;

	void					   endGame(EndGameState state, PlayerColor player = PlayerColor::None) override;
	void					   changeCurrentPlayer(PlayerColor player) override;
	PlayerColor				   getCurrentPlayer() const;

	void					   setLocalPlayer(PlayerColor player);
	PlayerColor				   getLocalPlayer() const;

	void					   switchTurns();

	bool					   calculateAllMovesForPlayer();

	bool					   initiateMove(const Position &startPosition);

	EndGameState			   checkForEndGameConditions();

	void					   setCPUConfiguration(const CPUConfiguration &config);

	CPUConfiguration		   getCPUConfiguration() const;

	bool					   isCPUPlayer(PlayerColor player) const;

	void					   requestCPUMoveAsync();


private:
	bool					  mMovesGeneratedForCurrentTurn = false;

	Player					  mWhitePlayer;
	Player					  mBlackPlayer;

	PlayerColor				  mCurrentPlayer = PlayerColor::None;

	std::vector<PossibleMove> mAllMovesForPosition;

	Chessboard				  mChessBoard;

	MoveGeneration			  mMoveGeneration;
	MoveExecution			  mMoveExecution;
	MoveEvaluation			  mMoveEvaluation;

	CPUPlayer				  mCPUPlayer;

	std::mutex				  mMutex;

	friend class GameManager;
};
