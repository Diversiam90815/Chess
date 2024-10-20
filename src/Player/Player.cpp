/*
  ==============================================================================

	Class:          Player

	Description:    Stats and settings for the player

  ==============================================================================
*/


#include "Player.h"


Player::Player()
{
}


Player::Player(PieceColor color) : mPlayerColor(color)
{
}


Player::~Player()
{
}


bool Player::isOnTurn() const
{
	return mIsCurrentTurn;
}


void Player::setOnTurn(bool value)
{
	if (mIsCurrentTurn != value)
	{
		mIsCurrentTurn = value;
	}
}


int Player::getScore() const
{
	return mScore;
}


void Player::setScore(int value)
{
	if (mScore != value)
	{
		mScore = value;
	}
}


PieceColor Player::getPlayerColor() const
{
	return mPlayerColor;
}


void Player::setPlayerColor(PieceColor value)
{
	if (mPlayerColor != value)
	{
		mPlayerColor = value;
	}
}
