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


class GameManager
{
public:
	GameManager();
	~GameManager();

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

private:
	void							 switchTurns();

	void							 handleMoveStateChanges(PossibleMove &move);

	void							 checkForEndGameConditions();


	Player							 mWhitePlayer;
	Player							 mBlackPlayer;

	PlayerColor						 mCurrentPlayer	   = PlayerColor::NoColor;
	GameState						 mCurrentState	   = GameState::Init;
	MoveState						 mCurrentMoveState = MoveState::NoMove;

	std::vector<PossibleMove>		 mAllMovesForPosition;

	std::unique_ptr<MovementManager> mMovementManager;
};
