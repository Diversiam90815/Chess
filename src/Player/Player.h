/*
  ==============================================================================

	Class:          Player

	Description:    Stats and settings for the player

  ==============================================================================
*/


#pragma once

#include "ChessPiece.h"


class Player
{
public:
	Player();

	Player(PieceColor color);

	~Player();


	bool isOnTurn() const;

	void setOnTurn(bool value);

	int	 getScore() const;
	void setScore(int value);

	PieceColor getPlayerColor() const;
	void	   setPlayerColor(PieceColor value);

private:

	PieceColor				mPlayerColor = PieceColor::None;

	std::vector<ChessPiece> mCapturedPieces;

	bool					mIsCurrentTurn = false;

	int						mScore		  = 0;
};
