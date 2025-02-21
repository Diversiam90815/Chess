/*
  ==============================================================================
	Module:         Player
	Description:    Stats and settings for the player
  ==============================================================================
*/


#pragma once

#include "ChessPiece.h"
#include "Parameters.h"
#include "Logging.h"

#include "ChessLogicAPIDefines.h"
#include "IObservable.h"


struct Score
{
	Score(PlayerColor player, int value) : player(player), score(value) {};

	PlayerColor player = PlayerColor::NoColor;
	int			score  = 0;

	bool		operator==(Score other) { return this->score == other.score && this->player == other.player; }
};

struct PlayerCapturedPiece
{
	PlayerColor playerColor;
	PieceType	pieceType;
	bool		captured; // False if we undo the move and remove a piece
};


class Player : public IPlayerObservable
{
public:
	Player();

	Player(PlayerColor color);

	~Player();


	Score		  getScore() const;
	void		  setScore(int value);

	PlayerColor	  getPlayerColor() const;
	void		  setPlayerColor(PlayerColor value);


	void		  addCapturedPiece(const PieceType piece) override;
	void		  removeLastCapturedPiece() override;

	void		  updateScore() override;

	constexpr int getPieceValue(PieceType piece);

	void		  reset();

	void		  setDelegate(PFN_CALLBACK pDelegate);

	void		  attachObserver(IPlayerObserver *observer) override;
	void		  detachObserver(IPlayerObserver *observer) override;

private:
	PlayerColor					   mPlayerColor = PlayerColor::NoColor;

	std::vector<PieceType>		   mCapturedPieces;

	bool						   mIsCurrentTurn = false;

	Score						   mScore		  = Score(PlayerColor::NoColor, 0);

	std::vector<IPlayerObserver *> mObservers;

	PFN_CALLBACK				   mDelegate = nullptr;
};
