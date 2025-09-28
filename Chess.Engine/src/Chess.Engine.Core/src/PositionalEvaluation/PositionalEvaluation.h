/*
  ==============================================================================
	Module:         PositionalEvaluation
	Description:    Positional evaluation for chess positions
  ==============================================================================
*/

#pragma once

#include "Parameters.h"
#include "LightChessBoard.h"
#include "Evaluation/MoveEvaluation.h"


/**
 * @brief	Decomposed evaluation components for diagnostic / tuning purposes.
 *
 * Perspective:
 *  - All component scores are computed from the perspective of the side passed
 *    into the evaluation routine; positive favors that side.
 *
 * Fields:
 *  - materialScore   : Pure material delta (may incorporate piece-square tables indirectly if desired).
 *  - positionalScore : Non-material strategic factors (outposts, space, piece activity not counted in mobility).
 *  - kingSafetyScore : Exposure / shelter / attack vectors impact around king(s).
 *  - mobilityScore   : Legal move count quality proxy (often scaled / normalized).
 *  - tacticalScore   : Immediate tactical motifs (pins, forks, threats) aggregated.
 *  - structuralScore : Pawn structure (isolated, doubled, passed, chains, majorities).
 *
 * Total:
 *  - `getTotalScore()` simply sums raw component scores. If weighting is phase-dependent,
 *    weighting occurs prior to populating this struct OR a parallel weighted sum is stored elsewhere.
 */
struct PositionalEvaluationResult
{
	int materialScore	= 0;
	int positionalScore = 0;
	int kingSafetyScore = 0;
	int mobilityScore	= 0;
	int tacticalScore	= 0;
	int structuralScore = 0;

	int getTotalScore() const { return materialScore + positionalScore + kingSafetyScore + mobilityScore + tacticalScore + structuralScore; }
};


/**
 * @brief	Phase-adjustable scalar multipliers applied to raw evaluation components.
 *
 * Usage:
 *  - Selected per game phase (opening / midgame / endgame) to emphasize contextually
 *    critical factors (e.g., king safety earlier, pawn structure later).
 *
 * Interpretation:
 *  - Each weight is a linear multiplier. Raw component * weight = weighted component.
 *  - Weight magnitudes are relative; absolute scaling only affects global evaluation scale.
 */
struct EvaluationWeights
{
	float materialWeight   = 1.0f;
	float positionalWeight = 0.3f;
	float kingSafetyWeight = 0.8f;
	float mobilityWeight   = 0.2f;
	float tacticalWeight   = 0.6f;
	float structuralWeight = 0.4f;
};


/**
 * @brief	Composite heuristic evaluation for a chess position combining material,
 *			positional features, mobility, king safety, pawn structure, and tactical potential.
 *
 * Usage:
 *  - `evaluatePosition()` returns a single scalar (score from the perspective of `player`).
 *  - `evaluatePositionDetailed()` returns a structured breakdown (for diagnostics / tuning).
 *
 * Design:
 *  - Phase-aware weighting (opening / midgame / endgame) via `determineGamePhase()`
 *    and `getWeightsForPhase()`.
 *  - Delegates move enumeration dependent heuristics to `MoveEvaluation` (e.g.,
 *    best-move opportunities, mobility).
 *
 * Conventions:
 *  - Positive score favors the passed `player`.
 *  - Material values assumed consistent with `getPieceValue()`.
 */
class PositionalEvaluation
{
public:
	explicit PositionalEvaluation(std::shared_ptr<MoveEvaluation> moveEvaluation);
	~PositionalEvaluation() = default;

	/**
	 * @brief	Compute aggregate evaluation score.
	 * @param	board -> Lightweight board snapshot.
	 * @param	player -> Perspective color.
	 * @return	Signed score (positive = advantage for player).
	 */
	int						   evaluatePosition(const LightChessBoard &board, PlayerColor player);

	/**
	 * @brief	Detailed multi-component evaluation.
	 * @param	board -> Lightweight board snapshot.
	 * @param	player -> Perspective color.
	 * @return	Struct capturing each term and final weighted sum.
	 */
	PositionalEvaluationResult evaluatePositionDetailed(const LightChessBoard &board, PlayerColor player);

	int						   evaluateMaterial(const LightChessBoard &board, PlayerColor player);
	int						   evaluatePositionalAdvantage(const LightChessBoard &board, PlayerColor player);
	int						   evaluateKingSafety(const LightChessBoard &board, PlayerColor player);
	int						   evaluateMobility(const LightChessBoard &board, PlayerColor player);
	int						   evaluateTacticalOpportunities(const LightChessBoard &board, PlayerColor player);
	int						   evaluatePawnStructure(const LightChessBoard &board, PlayerColor player);

private:
	/**
	 * @brief	Determine game phase (impacts weighting heuristics).
	 */
	GamePhase						determineGamePhase(const LightChessBoard &board) const;

	PlayerColor						getOpponent(PlayerColor player) const;

	/**
	 * @brief	Retrieve evaluation weight set for specified phase.
	 */
	EvaluationWeights				getWeightsForPhase(GamePhase phase) const;

	/**
	 * @brief	Evaluate how many top candidate moves (search-limited) exist,
	 *			rewarding broader high-quality choice sets.
	 * @param	board -> Lightweight board snapshot.
	 * @param	player -> Perspective color.
	 * @param	maxMoves -> Cap on enumerated moves for efficiency.
	 */
	int								evaluateBestMovesOpportunity(const LightChessBoard &board, PlayerColor player, int maxMoves);

	bool							hasPawnSupport(const LightChessBoard &board, const Position &pawnPos, PlayerColor player) const;
	int								evaluatePawnMajority(const LightChessBoard &board, PlayerColor player) const;
	int								evaluatePawnChains(const LightChessBoard &board, PlayerColor player) const;


	EvaluationWeights				mEvaluationWeights;

	std::shared_ptr<MoveEvaluation> mMoveEvaluation;
};
