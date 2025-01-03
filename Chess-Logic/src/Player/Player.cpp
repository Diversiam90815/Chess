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


Player::Player(PlayerColor color) : mPlayerColor(color)
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


Score Player::getScore() const
{
	return mScore;
}


void Player::setScore(int value)
{
	Score newScore = Score(this->getPlayerColor(), value);

	if (mScore != newScore)
	{
		mScore = newScore;

		if (mDelegate)
		{
			mDelegate(delegateMessage::playerScoreUpdated, &mScore);
		}
	}
}


PlayerColor Player::getPlayerColor() const
{
	return mPlayerColor;
}


void Player::setPlayerColor(PlayerColor value)
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

	LOG_INFO("Updated Score for {} : {}", LoggingHelper::playerColourToString(mPlayerColor).c_str(), score);
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


void Player::reset()
{
	setScore(0);
	mCapturedPieces.clear();
	setOnTurn(false);
}


void Player::setDelegate(PFN_CALLBACK pDelegate)
{
	mDelegate = pDelegate;
}
