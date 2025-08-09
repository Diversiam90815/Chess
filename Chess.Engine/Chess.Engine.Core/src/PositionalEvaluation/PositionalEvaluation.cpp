/*
  ==============================================================================
	Module:         PositionalEvaluation
	Description:    Positional evaluation for chess positions
  ==============================================================================
*/

#include "PositionalEvaluation.h"


PositionalEvaluation::PositionalEvaluation(std::shared_ptr<MoveEvaluation> moveEvaluation) : mMoveEvaluation(moveEvaluation) {}


PositionalEvaluation::~PositionalEvaluation() {}


int PositionalEvaluation::evaluatePosition(const LightChessBoard &board, PlayerColor player)
{
	return evaluatePositionDetailed(board, player).getTotalScore();
}


PositionalEvaluationResult PositionalEvaluation::evaluatePositionDetailed(const LightChessBoard &board, PlayerColor player)
{
	PositionalEvaluationResult result;
	GamePhase				   phase   = determineGamePhase(board);
	EvaluationWeights		   weights = getWeightsForPhase(phase);

	// Calculate individual components
	result.materialScore			   = static_cast<int>(evaluateMaterial(board, player) * weights.materialWeight);
	result.positionalScore			   = static_cast<int>(evaluatePositionalAdvantage(board, player) * weights.positionalWeight);
	result.kingSafetyScore			   = static_cast<int>(evaluateKingSafety(board, player) * weights.kingSafetyWeight);
	result.mobilityScore			   = static_cast<int>(evaluateMobility(board, player) * weights.mobilityWeight);
	result.tacticalScore			   = static_cast<int>(evaluateTacticalOpportunities(board, player) * weights.tacticalWeight);
	result.structuralScore			   = static_cast<int>(evaluatePawnStructure(board, player) * weights.structuralWeight);

	return result;
}


GamePhase PositionalEvaluation::determineGamePhase(const LightChessBoard &board) const
{
	return mMoveEvaluation->determineGamePhase(&board);
}


PlayerColor PositionalEvaluation::getOpponent(PlayerColor player) const
{
	return (player == PlayerColor::White) ? PlayerColor::Black : PlayerColor::White;
}


int PositionalEvaluation::evaluateMaterial(const LightChessBoard &board, PlayerColor player)
{
	PlayerColor opponent = getOpponent(player);
	return board.getMaterialValue(player) - board.getMaterialValue(opponent);
}


int PositionalEvaluation::evaluatePositionalAdvantage(const LightChessBoard &board, PlayerColor player)
{
	int			score		 = 0;
	PlayerColor oponnent	 = getOpponent(player);

	// Evaluate our pieces' postions
	auto		playerPieces = board.getPiecePositions(player);

	for (const auto &pos : playerPieces)
	{
		const auto &piece = board.getPiece(pos);

		if (piece.isEmpty())
			continue;

		score += mMoveEvaluation->getPositionValue(piece.type, pos, player, &board);
	}

	// Subtract oponnent's positions
	auto opponentPieces = board.getPiecePositions(oponnent);

	for (const auto &pos : opponentPieces)
	{
		const auto &piece = board.getPiece(pos);

		if (piece.isEmpty())
			continue;

		score -= mMoveEvaluation->getPositionValue(piece.type, pos, player, &board);
	}

	return score;
}


int PositionalEvaluation::evaluateKingSafety(const LightChessBoard &board, PlayerColor player)
{
	int			score	 = 0;
	GamePhase	phase	 = determineGamePhase(board);
	PlayerColor opponent = getOpponent(player);

	if (phase != GamePhase::EndGame)
	{
		// Focus on King safety
		if (board.isInCheck(player))
			score -= 50;
		if (board.isInCheck(opponent))
			score += 50;
	}
	else
	{
		// Encourage King centralization
		Position playerKingPos	 = board.getKingPosition(player);
		Position opponentKingPos = board.getKingPosition(opponent);

		score += mMoveEvaluation->getPositionValue(PieceType::King, playerKingPos, player, &board);
		score -= mMoveEvaluation->getPositionValue(PieceType::King, opponentKingPos, opponent, &board);
	}

	return score;
}


int PositionalEvaluation::evaluateMobility(const LightChessBoard &board, PlayerColor player)
{
	auto playerMoves   = board.generateLegalMoves(player);
	auto opponentMoves = board.generateLegalMoves(getOpponent(player));

	return static_cast<int>(playerMoves.size()) - static_cast<int>(opponentMoves.size());
}


int PositionalEvaluation::evaluateTacticalOpportunities(const LightChessBoard &board, PlayerColor player)
{
	return evaluateBestMovesOpportunity(board, player, 3) - evaluateBestMovesOpportunity(board, getOpponent(player), 3);
}


int PositionalEvaluation::evaluatePawnStructure(const LightChessBoard &board, PlayerColor player)
{
	// TODO
	return 0;
}


EvaluationWeights PositionalEvaluation::getWeightsForPhase(GamePhase phase) const
{
	EvaluationWeights weights;

	// Adjust GamePhase critical weight values
	switch (phase)
	{
	case GamePhase::Opening:
	{
		weights.kingSafetyWeight = 1.0f; // TODO: Adjust
		weights.mobilityWeight	 = 1.0f; // TODO: Adjust
		weights.tacticalWeight	 = 1.0f; // TODO: Adjust
		break;
	}
	case GamePhase::MiddleGame:
	{
		weights.kingSafetyWeight = 1.0f; // TODO: Adjust
		weights.mobilityWeight	 = 1.0f; // TODO: Adjust
		weights.tacticalWeight	 = 1.0f; // TODO: Adjust
		break;
	}
	case GamePhase::EndGame:
	{
		weights.kingSafetyWeight = 1.0f; // TODO: Adjust
		weights.mobilityWeight	 = 1.0f; // TODO: Adjust
		weights.tacticalWeight	 = 1.0f; // TODO: Adjust
		break;
	}
	}

	return weights;
}


int PositionalEvaluation::evaluateBestMovesOpportunity(const LightChessBoard &board, PlayerColor player, int maxMoves)
{
	auto moves = board.generateLegalMoves(player);

	if (moves.empty())
		return 0;

	int evaluatedMovesNum = std::min(maxMoves, static_cast<int>(moves.size()));
	int bestScore		  = -std::numeric_limits<int>::max();

	// Sort moves by basic evaluation
	std::partial_sort(moves.begin(), moves.begin() + evaluatedMovesNum, moves.end(),
					  [&](const PossibleMove &a, const PossibleMove &b) { return mMoveEvaluation->getBasicEvaluation(a) > mMoveEvaluation->getBasicEvaluation(b); });

	for (int i = 0; i < evaluatedMovesNum; ++i)
	{
		int moveScore = mMoveEvaluation->getAdvancedEvaluation(moves[i], player, &board);
		bestScore	  = std::max(bestScore, moveScore);
	}

	return bestScore != -std::numeric_limits<int>::max() ? bestScore : 0;
}
