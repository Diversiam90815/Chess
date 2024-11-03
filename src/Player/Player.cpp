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


void Player::addCapturedPiece(const PieceType piece)
{
	mCapturedPieces.push_back(piece);
}


void Player::updateScore()
{
	int score = 0;
	for (auto piece : mCapturedPieces)
	{
		score += getPieceValue(piece);
	}
	setScore(score);
}


constexpr int Player::getPieceValue(PieceType piece)
{
	switch (piece)
	{
	case PieceType::Pawn: return pawnValue;
	case PieceType::Knight: return knightValue;
	case PieceType::Bishop: return bishopValue;
	case PieceType::Rook: return rookValue;
	case PieceType::Queen: return queenValue;
	case PieceType::King: return kingValue;
	default: return 0;
	}
}
