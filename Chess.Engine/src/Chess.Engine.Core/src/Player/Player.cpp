/*
  ==============================================================================
	Module:         Player
	Description:    Stats and settings for the player
  ==============================================================================
*/


#include "Player.h"


Player::Player(Side color) : mSide(color) {}


Side Player::getPlayerColor() const
{
	return mSide;
}


void Player::setPlayerColor(Side value)
{
	if (mSide != value)
	{
		mSide = value;
	}
}


void Player::addCapturedPiece(const PieceType piece)
{
	mCapturedPieces.push_back(piece);

	for (auto &observer : mObservers)
	{
		auto obs = observer.lock();

		if (obs)
			obs->onAddCapturedPiece(getPlayerColor(), piece);
	}
}


void Player::removeLastCapturedPiece()
{
	if (mCapturedPieces.empty())
	{
		// Handle the case where there are no captured pieces to remove
		LOG_WARNING("No captured pieces to remove.");
		return;
	}

	// Retrieve and remove the last captured piece
	PieceType lastCapture = mCapturedPieces.back();
	mCapturedPieces.pop_back();

	for (auto &observer : mObservers)
	{
		auto obs = observer.lock();

		if (obs)
			obs->onRemoveLastCapturedPiece(getPlayerColor(), lastCapture);
	}
}


void Player::reset()
{
	mCapturedPieces.clear();
}
