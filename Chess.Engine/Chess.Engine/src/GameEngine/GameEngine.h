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
#include "Validation/MoveValidation.h"
#include "Evaluation/MoveEvaluation.h"
#include "CPUPlayer.h"
#include "IObservable.h"
#include "Parameters.h"


class GameEngine : public IGameObservable
{
public:
	GameEngine()  = default;
	~GameEngine() = default;

	void					   init();
	void					   reset();

	void					   startGame();
	void					   resetGame();

	void					   executeMove(PossibleMove &tmpMove, bool fromRemote = false);
	void					   undoMove();

	PieceType				   getCurrentPieceTypeAtPosition(const Position position);

	std::vector<PossibleMove>  getPossibleMoveForPosition();

	bool					   getBoardState(int boardState[BOARD_SIZE][BOARD_SIZE]);

	bool					   checkForValidMoves(const PossibleMove &move);
	bool					   checkForPawnPromotionMove(const PossibleMove &move);

	std::optional<PlayerColor> getWinner() const;

	void					   endGame(EndGameState state, PlayerColor player = PlayerColor::NoColor) override;
	void					   changeCurrentPlayer(PlayerColor player) override;
	PlayerColor				   getCurrentPlayer() const;

	void					   setLocalPlayer(PlayerColor player);
	PlayerColor				   getLocalPlayer() const;

	void					   switchTurns();

	bool					   calculateAllMovesForPlayer();

	bool					   initiateMove(const Position &startPosition);

	EndGameState			   checkForEndGameConditions();

	void					   setCPUConfiguration(const CPUConfiguration &config);
	void					   setWhiteCPUConfiguration(const CPUConfiguration &config);
	void					   setBlackCPUConfiguration(const CPUConfiguration &config);

	CPUConfiguration		   getCPUConfiguration() const;
	CPUConfiguration		   getWhiteCPUConfiguration() const;
	CPUConfiguration		   getBlackCPUConfiguration() const;

	bool					   isCPUPlayer(PlayerColor player) const;

	void					   requestCPUMoveAsync(PlayerColor player);


private:
	bool							mMovesGeneratedForCurrentTurn = false;

	Player							mWhitePlayer;
	Player							mBlackPlayer;

	PlayerColor						mCurrentPlayer = PlayerColor::NoColor;

	std::vector<PossibleMove>		mAllMovesForPosition;

	std::shared_ptr<ChessBoard>		mChessBoard;

	std::shared_ptr<MoveGeneration> mMoveGeneration;
	std::shared_ptr<MoveValidation> mMoveValidation;
	std::shared_ptr<MoveExecution>	mMoveExecution;
	std::shared_ptr<MoveEvaluation> mMoveEvaluation;

	std::shared_ptr<CPUPlayer>		mCPUPlayer;
	std::shared_ptr<CPUPlayer>		mWhiteCPUPlayer; // For testing different CPU Configurations
	std::shared_ptr<CPUPlayer>		mBlackCPUPlayer; // For testing different CPU Configurations

	friend class GameManager;
};
