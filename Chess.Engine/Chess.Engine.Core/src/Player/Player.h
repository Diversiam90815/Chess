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

#include "IObservable.h"


struct Score
{
	Score(PlayerColor player, int value) : player(player), value(value) {};


	int			getValue() const { return value; }
	PlayerColor getPlayerColor() const { return player; }

	bool		operator==(Score other) { return this->value == other.value && this->player == other.player; }

private:
	PlayerColor player = PlayerColor::NoColor;
	int			value  = 0;
};


class Player : public IPlayerObservable
{
public:
	Player() = default;
	Player(PlayerColor color);
	~Player() = default;

	Score		  getScore() const;
	void		  setScore(int value);

	PlayerColor	  getPlayerColor() const;
	void		  setPlayerColor(PlayerColor value);

	void		  addCapturedPiece(const PieceType piece) override;
	void		  removeLastCapturedPiece() override;

	void		  updateScore() override;

	constexpr int getPieceValue(PieceType piece);

	void		  reset();

	bool		  isLocalPlayer() const { return mIsLocalPlayer; }
	void		  setIsLocalPlayer(const bool isLocal) { mIsLocalPlayer = isLocal; }

private:
	PlayerColor			   mPlayerColor = PlayerColor::NoColor;

	std::vector<PieceType> mCapturedPieces;

	bool				   mIsCurrentTurn = false;

	Score				   mScore		  = Score(PlayerColor::NoColor, 0);

	bool				   mIsLocalPlayer{true}; // Default to local player in single-player mode
};
