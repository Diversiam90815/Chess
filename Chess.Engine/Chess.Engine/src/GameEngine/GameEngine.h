/*
  ==============================================================================
	Module:         GameEngine
	Description:    Engine of the chess game
  ==============================================================================
*/


#pragma once

#include <optional>

#include "MoveExecution.h"
#include "MoveGeneration.h"
#include "MoveValidation.h"
#include "IObservable.h"
#include "Parameters.h"


class GameEngine : public IGameObservable
{
public:
	GameEngine()  = default;
	~GameEngine() = default;

	void					   init();
	void					   reset();
	void					   resetGame();

	void					   executeMove(PossibleMove &tmpMove, bool fromRemote = false);

	void					   undoMove();

	PieceType				   getCurrentPieceTypeAtPosition(const Position position);

	std::vector<PossibleMove>  getPossibleMoveForPosition();


	bool					   getBoardState(int boardState[BOARD_SIZE][BOARD_SIZE]);

	bool					   checkForValidMoves(const PossibleMove &move);
	bool					   checkForPawnPromotionMove(const PossibleMove &move);

	std::optional<PlayerColor> getWinner() const;


	void					   changeCurrentPlayer(PlayerColor player) override;
	PlayerColor				   getCurrentPlayer() const;

	void					   endGame(EndGameState state, PlayerColor player = PlayerColor::NoColor) override;

	void					   startGame();


	void					   setLocalPlayer(PlayerColor player);
	PlayerColor				   getLocalPlayer() const;

	void					   switchTurns();

	bool					   calculateAllMovesForPlayer();

	bool					   initiateMove(const Position &startPosition);

	EndGameState			   checkForEndGameConditions();


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

	friend class GameManager;
};
