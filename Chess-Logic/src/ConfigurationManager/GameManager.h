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
#include "UserSettings.h"

#include "NetworkManager.h"


class GameManager
{
public:
	~GameManager() = default;


	static GameManager		  *GetInstance();
	static void				   ReleaseInstance();


	void					   init();

	void					   startGame();

	void					   executeMove(PossibleMove &move);

	void					   undoMove();

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

	void					   setCurrentPlayer(PlayerColor player);
	PlayerColor				   getCurrentPlayer() const;


	void					   setBoardTheme(std::string theme) { mUserSettings.setCurrentBoardTheme(theme); }

	std::string				   getBoardTheme() const { return mUserSettings.getCurrentBoardTheme(); }

	void					   setPieceTheme(std::string theme) { mUserSettings.setCurrentPieceTheme(theme); }

	std::string				   getPieceTheme() const { return mUserSettings.getCurrentPieceTheme(); }


private:
	GameManager();

	void							 switchTurns();

	void							 checkForEndGameConditions();

	Logging							 mLog;

	bool							 mMovesGeneratedForCurrentTurn = false;

	Player							 mWhitePlayer;
	Player							 mBlackPlayer;

	PlayerColor						 mCurrentPlayer	   = PlayerColor::NoColor;

	GameState						 mCurrentState	   = GameState::Init;

	MoveState						 mCurrentMoveState = MoveState::NoMove;

	std::vector<PossibleMove>		 mAllMovesForPosition;

	std::unique_ptr<MovementManager> mMovementManager;

	UserSettings					 mUserSettings;

	std::unique_ptr<NetworkManager>	 mNetwork;

	PFN_CALLBACK					 mDelegate = nullptr;
};
