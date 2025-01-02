/*
  ==============================================================================

	Class:          GameManager

	Description:    Manager for the Chess game

  ==============================================================================
*/


#pragma once

#include "MovementManager.h"
#include "Player.h"
#include <optional>

#include "ChessLogicAPIDefines.h"

#include "Logging.h"


class GameManager
{
public:
	~GameManager() = default;


	static GameManager		  *GetInstance();
	static void				   ReleaseInstance();


	void					   init();

	void					   executeMove(PossibleMove &move);

	void					   setCurrentGameState(GameState state);
	GameState				   getCurrentGameState() const;

	void					   setCurrentMoveState(MoveState state);
	MoveState				   getCurrentMoveState() const;

	void					   resetGame();

	void					   endGame() const;

	std::optional<PlayerColor> getWinner() const;

	void					   clearState();

	void					   setDelegate(PFN_CALLBACK pDelegate);

	PieceType				   getCurrentPieceTypeAtPosition(const Position position);

	std::vector<PossibleMove>  getPossibleMoveForPosition();

	bool					   getBoardState(int boardState[BOARD_SIZE][BOARD_SIZE]);

	void					   handleMoveStateChanges(PossibleMove &move);

private:
	GameManager();

	void							 switchTurns();

	void							 checkForEndGameConditions();


	Logging							 mLog;

	Player							 mWhitePlayer;
	Player							 mBlackPlayer;

	PlayerColor						 mCurrentPlayer	   = PlayerColor::NoColor;
	GameState						 mCurrentState	   = GameState::Init;
	MoveState						 mCurrentMoveState = MoveState::NoMove;

	std::vector<PossibleMove>		 mAllMovesForPosition;

	std::unique_ptr<MovementManager> mMovementManager;

	PFN_CALLBACK					 mDelegate;
};
