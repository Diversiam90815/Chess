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
	Score(PlayerColor player, int value) : player(player), value(value) {};

	PlayerColor player = PlayerColor::NoColor;
	int			value  = 0;

	bool		operator==(Score other) { return this->value == other.value && this->player == other.player; }
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

	void		  attachObserver(std::weak_ptr<IPlayerObserver> observer) override;
	void		  detachObserver(std::weak_ptr<IPlayerObserver> observer) override;

private:
	PlayerColor									mPlayerColor = PlayerColor::NoColor;

	std::vector<PieceType>						mCapturedPieces;

	bool										mIsCurrentTurn = false;

	Score										mScore		   = Score(PlayerColor::NoColor, 0);

	std::vector<std::weak_ptr<IPlayerObserver>> mObservers;
};
