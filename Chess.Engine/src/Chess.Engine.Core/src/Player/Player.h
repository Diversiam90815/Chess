/*
  ==============================================================================
	Module:         Player
	Description:    Stats and settings for the player
  ==============================================================================
*/


#pragma once

#include "Parameters.h"
#include "Logging.h"

#include "IObservable.h"


/**
 * @brief	Immutable pairing of a player color with a numeric evaluation (material or aggregate).
 *
 * Semantics:
 *  - Represents ONLY the numeric value; it does not auto-update. Owning code is responsible
 *    for recomputation (e.g., after captures) and replacing the value via Player::setScore().
 *  - Value convention: positive magnitudes only (sign not used to distinguish players; the
 *    PlayerColor field does that explicitly).
 *
 * Usage Patterns:
 *  - Stored inside Player for quick retrieval and UI display.
 *  - Can be compared directly with another Score (same color + value).
 *
 * Invariants:
 *  - `player` is always a valid PlayerColor (may be NoColor before initialization).
 *  - `value` is non-negative (not enforced programmatically; callers should avoid negatives).
 */
struct Score
{
	Score(Side player, int value) : player(player), value(value) {};


	int			getValue() const { return value; }
	Side getPlayerColor() const { return player; }

	bool		operator==(Score other) { return this->value == other.value && this->player == other.player; }

private:
	Side player = Side::None;
	int			value  = 0;
};


/**
 * @brief	Lightweight aggregation of per-player state (material score, captured pieces,
 *			color assignment, local/remote designation).
 *
 * Responsibilities:
 *  - Maintain incremental material score (updated via 'updateScore()' after captures).
 *  - Track captured opponent pieces for UI / analytics.
 *  - Distinguish local vs remote (used by multiplayer logic / input gating).
 *
 * Design Notes:
 *  - Score is stored as a 'Score' domain object (supports color association).
 *  - Does not enforce turn ownership; that is orchestrated by StateMachine.
 */
class Player : public IPlayerObservable
{
public:
	Player() = default;
	Player(Side color);
	~Player() = default;

	Score		  getScore() const;
	void		  setScore(int value);

	Side		  getPlayerColor() const;
	void		  setPlayerColor(Side value);

	/**
	 * @brief	Record a newly captured opponent piece.
	 */
	void		  addCapturedPiece(const PieceType piece) override;

	/**
	 * @brief	Remove the most recently captured piece (undo support).
	 */
	void		  removeLastCapturedPiece() override;

	/**
	 * @brief	Recalculate material score based on captured pieces (and/or board state
	 *			depending on implementation).
	 */
	void		  updateScore() override;

	constexpr int getPieceValue(PieceType piece);

	/**
	 * @brief	Reset player state to initial (fresh game).
	 */
	void		  reset();

	bool		  isLocalPlayer() const { return mIsLocalPlayer; }
	void		  setIsLocalPlayer(const bool isLocal) { mIsLocalPlayer = isLocal; }

private:
	Side				   mPlayerColor = Side::None;

	std::vector<PieceType> mCapturedPieces;

	bool				   mIsCurrentTurn = false;

	Score				   mScore		  = Score(Side::None, 0);

	bool				   mIsLocalPlayer{true}; // Default to local player in single-player mode
};
