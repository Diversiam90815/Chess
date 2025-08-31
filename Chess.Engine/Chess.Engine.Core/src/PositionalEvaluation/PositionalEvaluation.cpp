/*
  ==============================================================================
	Module:         PositionalEvaluation
	Description:    Positional evaluation for chess positions
  ==============================================================================
*/

#include "PositionalEvaluation.h"


PositionalEvaluation::PositionalEvaluation(std::shared_ptr<MoveEvaluation> moveEvaluation) : mMoveEvaluation(moveEvaluation) {}


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
			score -= CHECK_FACTOR;
		if (board.isInCheck(opponent))
			score += CHECK_FACTOR;
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
	int			score		  = 0;
	PlayerColor opponent	  = getOpponent(player);

	// Get all pawn positions for both player
	auto		playerPawns	  = board.getPiecePositions(player, PieceType::Pawn);
	auto		opponentPawns = board.getPiecePositions(opponent, PieceType::Pawn);

	// Evaluate our pawnStructure
	for (const auto &pawnPos : playerPawns)
	{
		// Passed pawns are very valuable
		if (mMoveEvaluation->isPasssedPawn(pawnPos, player))
		{
			score += PASSED_PAWN_FACTOR;

			// Advanced passed pawns are even more valuable
			int advancement = (player == PlayerColor::White) ? (7 - pawnPos.y) : pawnPos.y;
			score += advancement * PASSED_ADVANCED_FACTOR; // Bonus increases as pawn advances
		}

		// Isolated pawns are weak
		if (mMoveEvaluation->isIsolatedPawn(pawnPos, player))
			score -= ISOLATED_PAWN_FACTOR;

		// Doubled pawns are weak
		if (mMoveEvaluation->isDoublePawn(pawnPos, player))
			score -= DOUBLE_PAWN_FACTOR;

		// Evaluate pawn chains and support
		if (hasPawnSupport(board, pawnPos, player))
			score += SUPPORTED_PAWN_FACTOR; // Supported pawns are stronger

		// Central pawns are more valuable
		if (mMoveEvaluation->isInCenter(pawnPos))
			score += CENTRAL_PAWN_FACTOR;
	}


	// Subtract opponent's pawn structure advantages
	for (const auto &pawnPos : opponentPawns)
	{
		if (mMoveEvaluation->isPasssedPawn(pawnPos, opponent))
		{
			score -= PASSED_PAWN_FACTOR;
			int advancement = (opponent == PlayerColor::White) ? (7 - pawnPos.y) : pawnPos.y;
			score -= advancement * PASSED_ADVANCED_FACTOR;
		}

		if (mMoveEvaluation->isIsolatedPawn(pawnPos, opponent))
			score += ISOLATED_PAWN_FACTOR; // Opponent's weaknesses benefit us

		if (mMoveEvaluation->isDoublePawn(pawnPos, opponent))
			score += DOUBLE_PAWN_FACTOR;

		if (hasPawnSupport(board, pawnPos, opponent))
			score -= SUPPORTED_PAWN_FACTOR;

		if (mMoveEvaluation->isInCenter(pawnPos))
			score -= CENTRAL_PAWN_FACTOR;
	}

	score += evaluatePawnMajority(board, player);
	score += evaluatePawnChains(board, player);

	return score;
}


EvaluationWeights PositionalEvaluation::getWeightsForPhase(GamePhase phase) const
{
	EvaluationWeights weights;

	// Adjust GamePhase critical weight values
	switch (phase)
	{
	case GamePhase::Opening:
	{
		weights.kingSafetyWeight = 1.0f;
		weights.mobilityWeight	 = 0.3f;
		weights.tacticalWeight	 = 0.4f;
		break;
	}
	case GamePhase::MiddleGame:
	{
		weights.kingSafetyWeight = 0.8f;
		weights.mobilityWeight	 = 0.2f;
		weights.tacticalWeight	 = 0.8f;
		break;
	}
	case GamePhase::EndGame:
	{
		weights.kingSafetyWeight = 0.2f; // King activity, not safety
		weights.mobilityWeight	 = 0.4f;
		weights.tacticalWeight	 = 0.3f;
		weights.structuralWeight = 0.6f; // pawn structure is more important
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


bool PositionalEvaluation::hasPawnSupport(const LightChessBoard &board, const Position &pawnPos, PlayerColor player) const
{
	// Check if this pawn is defended by another pawn
	int		 direction = (player == PlayerColor::White) ? 1 : -1; // Directions pawns move

	// Check diagonal squares behind the pawn for supporting pawns
	Position leftSupport{pawnPos.x - 1, pawnPos.y + direction};
	Position rightSupport{pawnPos.x + 1, pawnPos.y + direction};

	if (leftSupport.isValid())
	{
		const auto &piece = board.getPiece(leftSupport);

		if (!piece.isEmpty() && piece.type == PieceType::Pawn && piece.color == player)
			return true;
	}

	if (rightSupport.isValid())
	{
		const auto &piece = board.getPiece(rightSupport);

		if (!piece.isEmpty() && piece.type == PieceType::Pawn && piece.color == player)
			return true;
	}

	return false;
}


int PositionalEvaluation::evaluatePawnMajority(const LightChessBoard &board, PlayerColor player) const
{
	int			score				   = 0;
	PlayerColor opponent			   = getOpponent(player);

	// Cound pawns kingside (e-f) and queenside (a-d)
	int			playerKingsidePawns	   = 0;
	int			playerQueensidePawns   = 0;
	int			opponentKingsidePawns  = 0;
	int			opponentQueensidePawns = 0;

	auto		playerPawns			   = board.getPiecePositions(player, PieceType::Pawn);
	auto		opponentPawns		   = board.getPiecePositions(opponent, PieceType::Pawn);

	for (const auto &pos : playerPawns)
	{
		if (pos.x >= 4) // Files e-h (kingside)
			playerKingsidePawns++;
		else			// Files a-d (queenside)
			playerQueensidePawns++;
	}

	for (const auto &pos : opponentPawns)
	{
		if (pos.x >= 4)
			opponentKingsidePawns++;
		else
			opponentQueensidePawns++;
	}


	// Bonus for pawn majority on each side
	if (playerKingsidePawns > opponentKingsidePawns)
		score += PAWN_MAJORITY_FACTOR;
	if (playerQueensidePawns > opponentQueensidePawns)
		score += PAWN_MAJORITY_FACTOR;

	// Penalty for opponent's majorities
	if (opponentKingsidePawns > playerKingsidePawns)
		score -= PAWN_MAJORITY_FACTOR;
	if (opponentQueensidePawns > playerQueensidePawns)
		score -= PAWN_MAJORITY_FACTOR;

	return score;
}


int PositionalEvaluation::evaluatePawnChains(const LightChessBoard &board, PlayerColor player) const
{
	int	 score		 = 0;
	auto playerPawns = board.getPiecePositions(player, PieceType::Pawn);

	// Look for connected pawns
	for (const auto &pawnPos : playerPawns)
	{
		int		 chainLength = 1;

		// check for connected pawns diagonally forward
		int		 direction	 = (player == PlayerColor::White) ? -1 : 1;

		// Count chain going forward
		Position checkPos	 = pawnPos;
		while (true)
		{
			checkPos.x += 1; // Check right diagonal
			checkPos.y += direction;

			if (!checkPos.isValid())
				break;

			const auto &piece = board.getPiece(checkPos);

			if (!piece.isEmpty() && piece.type == PieceType::Pawn && piece.color == player)
				chainLength++;
			else
				break;
		}

		// Reset and check left diagonal
		checkPos = pawnPos;
		while (true)
		{
			checkPos.x -= 1; // Check left diagonal
			checkPos.y += direction;

			if (!checkPos.isValid())
				break;

			const auto &piece = board.getPiece(checkPos);

			if (!piece.isEmpty() && piece.type == PieceType::Pawn && piece.color == player)
				chainLength++;
			else
				break;
		}

		// Bonus for longer chains
		if (chainLength >= 3)
			score += (chainLength - 2) * PAWN_CHAIN_FACTOR;
	}

	return score;
}
