/*
  ==============================================================================
	Module:         MoveEvaluation
	Description:    Evaluation of moves
  ==============================================================================
*/


#include "MoveEvaluation.h"


MoveEvaluation::MoveEvaluation(std::shared_ptr<ChessBoard> chessboard, std::shared_ptr<MoveGeneration> generation) : mBoard(chessboard), mGeneration(generation) {}


int MoveEvaluation::getBasicEvaluation(const PossibleMove &move)
{
	int score = 0;

	// Capture bonus
	if ((move.type & MoveType::Capture) == MoveType::Capture)
		score += CAPTURE_BONUS;

	// Check bonus
	if ((move.type & MoveType::Check) == MoveType::Check)
		score += CHECK_BONUS;

	// Checkmate bonus
	if ((move.type & MoveType::Checkmate) == MoveType::Checkmate)
		score += CHECKMATE_BONUS;

	// Promotion bonus
	if ((move.type & MoveType::PawnPromotion) == MoveType::PawnPromotion)
	{
		score += PROMOTION_BONUS;

		// Additional bonus for promoting to queen
		if (move.promotionPiece == PieceType::Queen)
			score += 100;
	}

	// Castling bonus
	if ((move.type & MoveType::CastlingKingside) == MoveType::CastlingKingside || (move.type & MoveType::CastlingQueenside) == MoveType::CastlingQueenside)
		score += CASTLE_BONUS;

	return score;
}


int MoveEvaluation::getMediumEvaluation(const PossibleMove &move, PlayerColor player, const LightChessBoard *lightBoard)
{
	int score = getBasicEvaluation(move);

	score += evaluateMaterialGain(move, lightBoard);		   // Material evaluation
	score += evaluatePositionalGain(move, player, lightBoard); // Positional evaluation
	score += evaluateCenterControl(move, player, lightBoard);  // Center COntrol
	score += evaluateKingSafety(move, player, lightBoard);	   // King Safety
	score += evaluatePieceActivity(move, player, lightBoard);  // Piece activity

	return score;
}


int MoveEvaluation::getAdvancedEvaluation(const PossibleMove &move, PlayerColor player, const LightChessBoard *lightBoard)
{
	int score = getMediumEvaluation(move, player, lightBoard);

	score += getTacticalEvaluation(move, player, lightBoard);	  // Advanced technical evaluation
	score += getStrategicEvaluation(move, player, lightBoard);	  // Advanced strategic evaluation
	score += evaluateThreatLevel(move, player, lightBoard);		  // Threat analysis
	score += evaluateDefensivePatterns(move, player, lightBoard); // Defensive patterns

	return score;
}


int MoveEvaluation::getPositionValue(PieceType piece, const Position &pos, PlayerColor player, const LightChessBoard *lightBoard) const
{
	// Ensure position is valid
	if (!pos.isValid())
		return 0;

	int row = pos.y;
	int col = pos.x;

	// For black pieces: flip the table
	if (player == PlayerColor::Black)
		row = 7 - row;

	GamePhase phase = determineGamePhase(lightBoard);

	switch (piece)
	{
	case PieceType::Pawn: return (phase == GamePhase::EndGame) ? PAWN_TABLE_EG[row][col] : PAWN_TABLE_MG[row][col];
	case PieceType::Knight: return (phase == GamePhase::EndGame) ? KNIGHT_TABLE_EG[row][col] : KNIGHT_TABLE_MG[row][col];
	case PieceType::Bishop: return BISHOP_TABLE[row][col];
	case PieceType::Rook: return ROOK_TABLE[row][col];
	case PieceType::Queen: return QUEEN_TABLE[row][col];
	case PieceType::King: return (phase == GamePhase::EndGame) ? KING_TABLE_EG[row][col] : KING_TABLE_MG[row][col]; ;
	default: return 0;
	}
}


int MoveEvaluation::evaluateMaterialGain(const PossibleMove &move, const LightChessBoard *lightBoard)
{
	if ((move.type & MoveType::Capture) != MoveType::Capture)
		return 0;

	PieceType capturedPieceType = getPieceTypeFromPosition(move.end, lightBoard);
	PieceType movingPieceType	= getPieceTypeFromPosition(move.start, lightBoard);

	int		  capturedValue		= getPieceValue(capturedPieceType);
	int		  movingValue		= getPieceValue(movingPieceType);

	return capturedValue + (::std::max)(0, (capturedValue - movingValue));
}


int MoveEvaluation::evaluatePositionalGain(const PossibleMove &move, PlayerColor player, const LightChessBoard *lightBoard)
{
	int		  positionalScore = 0;
	PieceType pieceType		  = getPieceTypeFromPosition(move.start, lightBoard);

	// Add positional value for destination
	positionalScore += getPositionValue(pieceType, move.end, player, lightBoard);

	// Subtract position value for leaving current square
	positionalScore -= getPositionValue(pieceType, move.start, player, lightBoard);

	return positionalScore;
}


int MoveEvaluation::evaluateThreatLevel(const PossibleMove &move, PlayerColor player, const LightChessBoard *lightBoard)
{
	int			score			= 0;

	// Check if this move creates threat to the opponent pieces
	auto		attackedSquares = getAttackedSquares(move.end, player, lightBoard);
	PlayerColor opponent		= getOpponentColor(player);

	for (const auto &square : attackedSquares)
	{
		PlayerColor pieceColor = PlayerColor::NoColor;
		PieceType	pieceType  = PieceType::DefaultType;

		pieceType			   = getPieceTypeFromPosition(square, lightBoard);
		pieceColor			   = getPieceColorFromPosition(square, lightBoard);

		if (pieceType == PieceType::DefaultType || pieceColor != opponent)
			continue;

		// Threatening valuable pieces gives more points
		int pieceValue = getPieceValue(pieceType);
		score += pieceValue / 10; // Threat is worth 1/10th of piece value
	}

	// Check if this moves blocks opponnent threats
	if (blocksEnemyThreats(move, player, lightBoard))
		score += THREAT_BLOCK_BONUS;

	return score;
}


int MoveEvaluation::evaluateKingSafety(const PossibleMove &move, PlayerColor player, const LightChessBoard *lightBoard)
{
	int score = 0;

	if (determineGamePhase(lightBoard) != GamePhase::EndGame)
	{
		Position kingPos		 = lightBoard ? lightBoard->getKingPosition(player) : mBoard->getKingsPosition(player);
		Position opponentKingPos = lightBoard ? lightBoard->getKingPosition(getOpponentColor(player)) : mBoard->getKingsPosition(getOpponentColor(player));

		// Penalize moves that expose our king
		if (wouldExposeKing(move, player, lightBoard))
			score -= EXPOSE_KING_FACTOR;

		// Reward moves that attack near the opponnents king
		if (isNearKing(move.end, opponentKingPos))
			score += ATTACK_NEAR_KING_FACTOR;

		// Reward defensive moves near our king when under threat
		if (isNearKing(move.end, kingPos))
		{
			int attackerCount = countAttackers(kingPos, getOpponentColor(player), lightBoard);
			if (attackerCount > 0)
				score += DEFENDING_KING_FACTOR;
		}

		score = score * KING_SAFETY_WEIGHT;
	}
	else
	{
		// In endgame, reward king activity and centralization
		PieceType pieceType = getPieceTypeFromPosition(move.start);

		if (pieceType != PieceType::King)
			return 0;

		// Reward king moving towards center in endgame
		int centerDistance	= abs(move.end.x - 3.5) + abs(move.end.y - 3.5);
		int currentDistance = abs(move.start.x - 3.5) + abs(move.start.y - 3.5);

		if (centerDistance < currentDistance)
			score += KING_CENTRALIZATION_FACTOR; // Bonus for king centralization in endgame

		// Additional bonus for king activity (any king move in endgame)
		score += KING_ACTIVITY_FACTOR;
	}

	return score;
}


int MoveEvaluation::evaluateCenterControl(const PossibleMove &move, PlayerColor player, const LightChessBoard *lightBoard)
{
	int score = 0;

	// Reward moves that control/occupy center squares
	if (isInCenter(move.end))
		score += CENTER_CONTROL_BONUS;

	// Additional: bonus for pieces attacking center squares
	auto attackedSquares = getAttackedSquares(move.end, player, lightBoard);

	for (const auto &square : attackedSquares)
	{
		if (isInCenter(square))
			score += CENTER_CONTROL_BONUS / 2;
	}

	return score;
}


int MoveEvaluation::evaluatePawnStructure(const PossibleMove &move, PlayerColor player, const LightChessBoard *lightBoard)
{
	int		  score = 0;
	PieceType type	= getPieceTypeFromPosition(move.start, lightBoard);

	if (type != PieceType::Pawn)
		return 0;

	// Check for pawn structure improvements
	if (isPasssedPawn(move.end, player))
		score += PASSED_PAWN_FACTOR;

	if (isIsolatedPawn(move.end, player))
		score -= ISOLATED_PAWN_FACTOR;

	if (isDoublePawn(move.end, player))
		score -= DOUBLE_PAWN_FACTOR;

	return score;
}


int MoveEvaluation::evaluatePieceActivity(const PossibleMove &move, PlayerColor player, const LightChessBoard *lightBoard)
{
	PieceType pieceType = getPieceTypeFromPosition(move.start, lightBoard);

	if (pieceType == PieceType::DefaultType)
		return 0;

	if (lightBoard)
	{
		// Fast path using LightChessBoard
		LightChessBoard tmpBoard = *lightBoard;
		auto			undoInfo = tmpBoard.makeMove(move);

		// Generate moves from new position
		auto			moves	 = tmpBoard.generateLegalMoves(player);
		int				mobility = 0;

		// Count moves from the destination square
		for (const auto &testMove : moves)
		{
			if (testMove.start == move.end)
				mobility++;
		}

		tmpBoard.unmakeMove(undoInfo);

		return mobility * 2;
	}
	else
	{
		// Fallback to ChessBoard (existing implementation)
		auto &piece = mBoard->getPiece(move.start);

		if (!piece)
			return 0;

		ChessBoard tmpBoard(*mBoard);
		tmpBoard.movePiece(move.start, move.end);

		auto moves = piece->getPossibleMoves(move.end, tmpBoard, true);

		return static_cast<int>(moves.size()) * 2;
	}
}


int MoveEvaluation::evaluateDefensivePatterns(const PossibleMove &move, PlayerColor player, const LightChessBoard *lightBoard)
{
	int		 score	 = 0;

	// Check if move defends important pieces or squares
	Position kingPos = lightBoard ? lightBoard->getKingPosition(player) : mBoard->getKingsPosition(player);

	// Defending the king area
	if (isNearKing(move.end, kingPos))
		score += DEFENDING_KING_FACTOR;

	// Defending important central squares
	if (isInCenter(move.end))
		score += CENTER_CONTROL_BONUS;

	// Check if move blocks enemy attacks on our pieces
	auto attackedSquares = getAttackedSquares(move.end, player, lightBoard);

	for (const auto &square : attackedSquares)
	{
		PlayerColor pieceColor = getPieceColorFromPosition(square, lightBoard);

		if (pieceColor == player)
			score += BLOCK_ATTACK_FACTOR;
	}

	return score;
}


bool MoveEvaluation::createsPin(const PossibleMove &move, PlayerColor player, const LightChessBoard *lightBoard)
{
	PieceType movingPieceType = getPieceTypeFromPosition(move.start, lightBoard);

	// Only long range pieces (rook, bishop and queen) can create pins
	if (movingPieceType != PieceType::Bishop && movingPieceType != PieceType::Rook && movingPieceType != PieceType::Queen)
		return false;

	PlayerColor opponent	 = getOpponentColor(player);
	Position	opponentKing = lightBoard ? lightBoard->getKingPosition(opponent) : mBoard->getKingsPosition(opponent);

	// Check if the piece after moving would be on the same line as the opponent's king
	if (!areCollinear(move.end, opponentKing, movingPieceType))
		return false;

	// Determine direction from our piece to opponnent's king
	int dx	  = opponentKing.x - move.end.x;
	int dy	  = opponentKing.y - move.end.y;

	// Normalize direction for sliding pieces
	int stepX = 0, stepY = 0;
	if (dx != 0)
		stepX = dx / abs(dx);

	if (dy != 0)
		stepY = dy / abs(dy);

	// Count pieces in between
	int		 piecesInBetween = 0;
	Position current		 = move.end;
	current.x += stepX;
	current.y += stepY;

	while (current.isValid() && current != opponentKing)
	{
		PlayerColor pieceColor = getPieceColorFromPosition(current, lightBoard);

		if (pieceColor == opponent)
			piecesInBetween++;

		current.x += stepX;
		current.y += stepY;
	}

	// Pin exists if there is excactly one (opponnent's) piece between us and the king
	return piecesInBetween == 1;
}


bool MoveEvaluation::createsFork(const PossibleMove &move, PlayerColor player, const LightChessBoard *lightBoard)
{
	PieceType movingPieceType = getPieceTypeFromPosition(move.start, lightBoard);
	if (movingPieceType == PieceType::DefaultType)
		return false;

	PlayerColor			  opponent		  = getOpponentColor(player);
	int					  valuableTargets = 0;

	std::vector<Position> attackedSquares;

	if (lightBoard)
	{
		// Create a temporary LightChessBoard to simulate the move
		LightChessBoard tmpBoard = *lightBoard;
		auto			undoInfo = tmpBoard.makeMove(move);

		// Generate attacked squares from new position
		attackedSquares			 = getAttackedSquares(move.end, player, &tmpBoard);

		tmpBoard.unmakeMove(undoInfo);
	}
	else
	{
		// Fallback to ChessBoard method
		ChessBoard tmpBoard(*mBoard);
		tmpBoard.movePiece(move.start, move.end);

		auto &piece = mBoard->getPiece(move.start);

		if (!piece)
			return false;

		auto possibleMoves = piece->getPossibleMoves(move.end, tmpBoard, true);
		for (const auto &attackMove : possibleMoves)
		{
			attackedSquares.push_back(attackMove.end);
		}
	}

	// Count valuable targets
	for (const auto &attackedPos : attackedSquares)
	{
		PieceType	targetType	= getPieceTypeFromPosition(attackedPos, lightBoard);
		PlayerColor targetColor = getPieceColorFromPosition(attackedPos, lightBoard);

		if (targetColor != opponent || targetType == PieceType::DefaultType)
			continue;

		// Count valuable pieces
		if (targetType == PieceType::Knight || targetType == PieceType::Bishop || targetType == PieceType::Rook || targetType == PieceType::Queen || targetType == PieceType::King)
		{
			valuableTargets++;
		}
	}

	return valuableTargets >= 2; // Fork if attacking 2+ valuable pieces
}


bool MoveEvaluation::createsSkewer(const PossibleMove &move, PlayerColor player, const LightChessBoard *lightBoard)
{
	PlayerColor opponent	= getOpponentColor(player);
	PieceType	movingPiece = getPieceTypeFromPosition(move.start, lightBoard);

	// Only long range pieces can create skewers
	if (movingPiece != PieceType::Bishop && movingPiece != PieceType::Rook && movingPiece != PieceType::Queen)
		return false;

	// Define directions depending on piece type
	std::vector<std::pair<int, int>> directions;

	if (movingPiece == PieceType::Rook || movingPiece == PieceType::Queen)
		directions = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};						   // rank & file
	if (movingPiece == PieceType::Bishop || movingPiece == PieceType::Queen)
		directions.insert(directions.end(), {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}}); // diagonal

	// Check each direction for potential skewers
	for (const auto &[dx, dy] : directions)
	{
		Position current = move.end;
		current.x += dx;
		current.y += dy;

		PieceType	firstPieceType	 = PieceType::DefaultType;
		PieceType	secondPieceType	 = PieceType::DefaultType;
		PlayerColor firstPieceColor	 = PlayerColor::NoColor;
		PlayerColor secondPieceColor = PlayerColor::NoColor;

		// Find the first two pieces in this direction
		while (current.isValid())
		{
			PieceType	currentPieceType  = getPieceTypeFromPosition(current, lightBoard);
			PlayerColor currentPieceColor = getPieceColorFromPosition(current, lightBoard);

			if (currentPieceType != PieceType::DefaultType)
			{
				if (firstPieceType == PieceType::DefaultType)
				{
					// Found the first piece in this direction
					firstPieceType	= currentPieceType;
					firstPieceColor = currentPieceColor;
				}
				else
				{
					// Found the second piece in this direction
					secondPieceType	 = currentPieceType;
					secondPieceColor = currentPieceColor;
					break;
				}
			}

			current.x += dx;
			current.y += dy;
		}

		// Check if we have a valid skewer pattern
		if (firstPieceType != PieceType::DefaultType && secondPieceType != PieceType::DefaultType && firstPieceColor == opponent && secondPieceColor == opponent)
		{
			int firstPieceValue	 = getPieceValue(firstPieceType);
			int secondPieceValue = getPieceValue(secondPieceType);

			if (firstPieceValue > secondPieceValue)
				return true;
		}
	}
	return false;
}


bool MoveEvaluation::blocksEnemyThreats(const PossibleMove &move, PlayerColor player, const LightChessBoard *lightBoard)
{
	PlayerColor opponent			 = getOpponentColor(player);
	Position	ourKing				 = lightBoard ? lightBoard->getKingPosition(player) : mBoard->getKingsPosition(player);

	// calculate threats in parallel

	auto		currentThreatsFuture = std::async(std::launch::async, [this, opponent, player, lightBoard]() { return calculateCurrentThreats(opponent, player, lightBoard); });

	auto		threatsAfterMoveFuture =
		std::async(std::launch::async, [this, move, opponent, player, lightBoard]() { return calculateThreatsAfterMove(move, player, opponent, lightBoard); });

	// Get results from both futures
	auto currentThreats	  = currentThreatsFuture.get();
	auto threatsAfterMove = threatsAfterMoveFuture.get();

	// Analyze the results
	return analyzeThreatReduction(currentThreats, threatsAfterMove, ourKing, move, player);
}


int MoveEvaluation::getStrategicEvaluation(const PossibleMove &move, PlayerColor player, const LightChessBoard *lightBoard)
{
	int score = 0;

	score += evaluatePawnStructure(move, player, lightBoard); // Pawn structure considerations

	score += evaluateKingSafety(move, player, lightBoard);	  // King safety in Early Game and Activation in Endgame

	score += evaluatePieceActivity(move, player, lightBoard); // Piece coordination and activity

	return score;
}


int MoveEvaluation::getTacticalEvaluation(const PossibleMove &move, PlayerColor player, const LightChessBoard *lightBoard)
{
	int score = 0;

	if (createsFork(move, player, lightBoard))
		score += FORK_BONUS;

	if (createsPin(move, player, lightBoard))
		score += PIN_BONUS;

	if (createsSkewer(move, player, lightBoard))
		score += SKEWER_BONUS;

	score += evaluateThreatLevel(move, player, lightBoard) * THREAT_WEIGHT;

	return score;
}


GamePhase MoveEvaluation::determineGamePhase(const LightChessBoard *lightBoard) const
{
	if (lightBoard)
		return static_cast<GamePhase>(lightBoard->getGamePhaseValue());

	int totalMaterial = 0;
	int totalPieces	  = 0;

	// Count total material on board
	for (int y = 0; y < BOARD_SIZE; ++y)
	{
		for (int x = 0; x < BOARD_SIZE; ++x)
		{
			Position pos{x, y};
			auto	&piece = mBoard->getPiece(pos);

			if (!piece)
				continue;

			totalMaterial += getPieceValue(piece->getType());
			totalPieces++;
		}
	}

	// Determine game phase based on material and piece count
	if (totalMaterial > OPENING_MATERIAL_THRESHOLD || totalPieces > OPENING_PIECE_THRESHOLD)
		return GamePhase::Opening;

	else if (totalMaterial > MIDDLEGAME_MATERIAL_THRESHOLD || totalPieces > MIDDLEGAME_PIECE_THRESHOLD)
		return GamePhase::MiddleGame;

	else
		return GamePhase::EndGame;
}


MoveEvaluation::ThreatAnalysis MoveEvaluation::calculateCurrentThreats(PlayerColor opponent, PlayerColor player, const LightChessBoard *lightBoard)
{
	std::vector<Position> threats;
	Position			  ourKing = lightBoard ? lightBoard->getKingPosition(player) : mBoard->getKingsPosition(player);

	if (lightBoard)
	{
		auto opponentPieces = lightBoard->getPiecePositions(opponent);

		for (const auto &pos : opponentPieces)
		{
			// Generate moves for this opponent piece
			auto moves = lightBoard->generateLegalMoves(opponent);

			// Filter moves that start from this position
			for (const auto &move : moves)
			{
				if (move.start == pos)
				{
					// Check if move threatens one of our pieces
					PlayerColor threatenedPieceColor = getPieceColorFromPosition(move.end, lightBoard);

					if (threatenedPieceColor == player)
					{
						threats.push_back(move.end);
					}
				}
			}
		}
	}
	else
	{
		mGeneration->calculateAllLegalBasicMoves(opponent);
		auto opponentPieces = mBoard->getPiecesFromPlayer(opponent);

		for (const auto &[pos, piece] : opponentPieces)
		{
			auto moves = mGeneration->getMovesForPosition(pos);

			for (const auto &move : moves)
			{
				auto &threatenedPiece = mBoard->getPiece(move.end);

				if (threatenedPiece && threatenedPiece->getColor() == player)
				{
					threats.push_back(move.end);
				}
			}
		}
	}

	return ThreatAnalysis(threats, ourKing);
}


MoveEvaluation::ThreatAnalysis MoveEvaluation::calculateThreatsAfterMove(const PossibleMove &move, PlayerColor player, PlayerColor opponent, const LightChessBoard *lightBoard)
{
	Position			  ourKingAfterMove = lightBoard ? lightBoard->getKingPosition(player) : mBoard->getKingsPosition(player);
	std::vector<Position> threats;

	if (lightBoard)
	{
		LightChessBoard tmpBoard = *lightBoard;
		auto			undoInfo = tmpBoard.makeMove(move);

		// Calculate threats on the temporary board
		threats					 = calculateThreatsOnBoard(opponent, player, tmpBoard);
	}
	else
	{
		// Create a chessboard copy for simulating move
		ChessBoard tmpBoard(*mBoard);
		tmpBoard.movePiece(move.start, move.end);

		threats = calculateThreatsOnBoard(opponent, player, tmpBoard);
	}

	return ThreatAnalysis(threats, ourKingAfterMove);
}


bool MoveEvaluation::analyzeThreatReduction(const ThreatAnalysis &before, const ThreatAnalysis &after, const Position &ourKing, const PossibleMove &move, PlayerColor player)
{
	bool reducedKingThreats	 = after.kingThreats < before.kingThreats;

	bool reducedTotalThreats = after.threatenedPieces.size() < before.threatenedPieces.size();

	bool physicalBlock		 = physicallyBlocksAttack(move, player, *mBoard);

	return reducedKingThreats || reducedTotalThreats || physicalBlock;
}


bool MoveEvaluation::physicallyBlocksAttack(const PossibleMove &move, PlayerColor player, ChessBoard &board)
{
	Position	ourKing		   = board.getKingsPosition(player);
	PlayerColor oponent		   = getOpponentColor(player);
	auto		opponentPieces = board.getPiecesFromPlayer(oponent);

	for (const auto &[enemyPos, piece] : opponentPieces)
	{
		PieceType enemyType = piece->getType();

		// Only check long range pieces that can be blocked
		if (enemyType == PieceType::Bishop || enemyType == PieceType::Rook || enemyType == PieceType::Queen)
		{
			// Check if our move destination blocks the line between enemy and king
			if (!areCollinear(enemyPos, ourKing, enemyType))
				continue;

			// Check if the destination is in between
			int dx1			= move.end.x - enemyPos.x;
			int dy1			= move.end.y - enemyPos.y;
			int dx2			= ourKing.x - enemyPos.x;
			int dy2			= ourKing.y - enemyPos.y;

			int distToEnemy = abs(dx1) + abs(dy1);
			int distToKing	= abs(dx2) + abs(dy2);

			if (distToEnemy < distToKing)
				return true;
		}
	}

	return false;
}


std::vector<Position> MoveEvaluation::calculateThreatsOnBoard(PlayerColor opponent, PlayerColor player, ChessBoard &board)
{
	// Use lightweight move generation without proper validation

	std::vector<Position> threats;
	auto				  opponentPieces = board.getPiecesFromPlayer(opponent);

	for (const auto &[pos, piece] : opponentPieces)
	{
		auto moves = piece->getPossibleMoves(pos, board, true);

		for (const auto &move : moves)
		{
			auto &threatenedPiece = board.getPiece(move.end);

			if (threatenedPiece && threatenedPiece->getColor() == player)
			{
				threats.push_back(move.end);
			}
		}
	}

	return threats;
}


std::vector<Position> MoveEvaluation::calculateThreatsOnBoard(PlayerColor opponent, PlayerColor player, LightChessBoard &board)
{
	std::vector<Position> threats;
	auto				  opponentPieces = board.getPiecePositions(opponent);

	for (const auto &pos : opponentPieces)
	{
		// Generate all legal moves for the opponent
		auto moves = board.generateLegalMoves(opponent);

		// Filter moves that start from this piece position
		for (const auto &move : moves)
		{
			if (move.start == pos)
			{
				// Check if this move threatens one of our pieces
				auto &threatenedPiece = board.getPiece(move.end);

				if (!threatenedPiece.isEmpty() && threatenedPiece.color == player)
				{
					threats.push_back(move.end);
				}
			}
		}
	}

	return threats;
}


int MoveEvaluation::calculateMobility(PlayerColor player) const
{
	int	 mobility	  = 0;

	auto playerPieces = mBoard->getPiecesFromPlayer(player);

	for (const auto &[position, piece] : playerPieces)
	{
		auto attackedSquares = getAttackedSquares(position, player);
		mobility += static_cast<int>(attackedSquares.size());
	}

	return mobility;
}


int MoveEvaluation::calculateKingSafetyScore(PlayerColor player) const
{
	int			score	  = 0;

	Position	kingPos	  = mBoard->getKingsPosition(player);

	// Count attackers near king
	PlayerColor opponent  = getOpponentColor(player);
	int			attackers = countAttackers(kingPos, opponent);

	score -= ATTACKER_KING_FACTOR * attackers; // Penalty for each attacker

	// bonus for each king protecting pieces near king
	for (int dx = -1; dx <= 1; ++dx)
	{
		for (int dy = -1; dy <= 1; ++dy)
		{
			Position nearPos{kingPos.x + dx, kingPos.y + dy};

			if (!nearPos.isValid())
				continue;

			auto &piece = mBoard->getPiece(nearPos);

			if (piece && piece->getColor() == player)
				score += DEFENDING_KING_FACTOR; // Bonus for friendly pieces near king
		}
	}

	return score;
}


int MoveEvaluation::calculatePawnStructureScore(PlayerColor player) const
{
	int	 score		  = 0;

	auto playerPieces = mBoard->getPiecesFromPlayer(player);

	for (const auto &[position, piece] : playerPieces)
	{
		if (piece->getType() != PieceType::Pawn)
			continue;

		if (isPasssedPawn(position, player))
			score += PASSED_PAWN_FACTOR;

		if (isIsolatedPawn(position, player))
			score -= ISOLATED_PAWN_FACTOR;

		if (isDoublePawn(position, player))
			score -= DOUBLE_PAWN_FACTOR;
	}

	return score;
}


bool MoveEvaluation::isPasssedPawn(const Position &pos, PlayerColor player) const
{
	// Check if no enemy pawn can block this pawn's advance
	int direction = (player == PlayerColor::White) ? -1 : 1;
	int startY	  = pos.y;
	int endY	  = (player == PlayerColor::White) ? 0 : 7;

	for (int y = startY + direction; y != endY; y += direction)
	{
		Position checkPos{pos.x, y};
		auto	&piece = mBoard->getPiece(checkPos);

		if (piece && piece->getType() == PieceType::Pawn && piece->getColor() != player)
			return false; // Found enemy pawn that could block
	}

	return true;
}


bool MoveEvaluation::isIsolatedPawn(const Position &pos, PlayerColor player) const
{
	// Check adjacent files for friendly pawns
	for (int fileOffset = -1; fileOffset <= 1; fileOffset += 2) // Check left and right files
	{
		int checkFile = pos.x + fileOffset;
		if (checkFile < 0 || checkFile > 7)
			continue;

		// Check entire file for friendly pawns
		for (int y = 0; y < 8; ++y)
		{
			Position checkPos{checkFile, y};
			auto	&piece = mBoard->getPiece(checkPos);

			if (piece && piece->getType() == PieceType::Pawn && piece->getColor() == player)
				return false; // Found friendly pawn on adjacent file
		}
	}

	return true;
}


bool MoveEvaluation::isDoublePawn(const Position &pos, PlayerColor player) const
{
	// Check same file for another friendly pawn
	for (int y = 0; y < BOARD_SIZE; ++y)
	{
		if (y == pos.y)
			continue; // Skip current position

		Position checkPos{pos.x, y};

		auto	&piece = mBoard->getPiece(checkPos);

		if (piece && piece->getType() == PieceType::Pawn && piece->getColor() == player)
			return true; // Found another friendly pawn pawn on same file
	}

	return false;
}


bool MoveEvaluation::isInCenter(const Position &pos) const
{
	return (pos.x >= 2 && pos.x <= 5 && pos.y >= 2 && pos.y <= 5);
}


bool MoveEvaluation::isNearKing(const Position &pos, const Position &kingPos) const
{
	return (abs(pos.x - kingPos.x) <= 2 && abs(pos.y - kingPos.y) <= 2);
}


std::vector<Position> MoveEvaluation::getAttackedSquares(const Position &piecePos, PlayerColor player, const LightChessBoard *lightBoard) const
{
	std::vector<Position>	  attackedSquares;
	std::vector<PossibleMove> moves;

	if (lightBoard)
	{
		auto allMoves = lightBoard->generateLegalMoves(player);

		for (const auto &move : allMoves)
		{
			// filter this piece's moves
			if (move.start == piecePos)
				moves.push_back(move);
		}
	}
	else
	{
		mGeneration->calculateAllLegalBasicMoves(player);
		moves = mGeneration->getMovesForPosition(piecePos);
	}

	attackedSquares.reserve(moves.size());

	for (const auto &move : moves)
	{
		attackedSquares.push_back(move.end);
	}

	return attackedSquares;
}


bool MoveEvaluation::wouldExposeKing(const PossibleMove &move, PlayerColor player, const LightChessBoard *lightBoard) const
{
	Position kingPos = lightBoard ? lightBoard->getKingPosition(player) : mBoard->getKingsPosition(player);

	// If we're moving a piece from near the king, check if it exposes the king
	if (isNearKing(move.start, kingPos) && !isNearKing(move.end, kingPos))
	{
		// Count current attackers
		int currentAttackers = countAttackers(kingPos, getOpponentColor(player), lightBoard);
		return currentAttackers > 0; // Risk if already under attack
	}

	return false;
}


int MoveEvaluation::countAttackers(const Position &target, PlayerColor attackerPlayer, const LightChessBoard *lightBoard) const
{
	int count = 0;

	for (int y = 0; y < BOARD_SIZE; ++y)
	{
		for (int x = 0; x < BOARD_SIZE; ++x)
		{
			const Position pos{x, y};
			PlayerColor	   pieceColor = getPieceColorFromPosition(pos, lightBoard);

			if (pieceColor != attackerPlayer)
				continue;

			auto attackedSquares = getAttackedSquares(pos, attackerPlayer, lightBoard);

			for (const auto &square : attackedSquares)
			{
				if (square == target)
				{
					count++;
					break;
				}
			}
		}
	}

	return count;
}


PlayerColor MoveEvaluation::getOpponentColor(PlayerColor player) const
{
	return (player == PlayerColor::White) ? PlayerColor::Black : PlayerColor::White;
}


bool MoveEvaluation::areCollinear(const Position &pos1, const Position &pos2, PieceType pieceType)
{
	int dx = pos2.x - pos1.x;
	int dy = pos2.y - pos1.y;

	switch (pieceType)
	{
	case PieceType::Bishop: return (dx != 0 && dy != 0 && abs(dx) == abs(dy));
	case PieceType::Rook: return (dx == 0 || dy == 0);
	case PieceType::Queen: return (dx == 0 || dy == 0 || (dx != 0 && dy != 0 && abs(dx) == abs(dy)));
	default: return false;
	}
}


PieceType MoveEvaluation::getPieceTypeFromPosition(const Position &pos, const LightChessBoard *lightBoard) const
{
	if (lightBoard)
	{
		auto &piece = lightBoard->getPiece(pos);

		if (piece.isValid())
			return piece.type;
	}
	else
	{
		auto &piece = mBoard->getPiece(pos);

		if (piece)
			return piece->getType();
	}

	return PieceType::DefaultType;
}


PlayerColor MoveEvaluation::getPieceColorFromPosition(const Position &pos, const LightChessBoard *lightBoard) const
{
	if (lightBoard)
	{
		auto &piece = lightBoard->getPiece(pos);

		if (piece.isValid())
			return piece.color;
	}
	else
	{
		auto &piece = mBoard->getPiece(pos);

		if (piece)
			return piece->getColor();
	}

	return PlayerColor::NoColor;
}


constexpr int MoveEvaluation::getPieceValue(PieceType piece)
{
	switch (piece)
	{
	case PieceType::Pawn: return PAWN_VALUE;
	case PieceType::Knight: return KNIGHT_VALUE;
	case PieceType::Bishop: return BISHOP_VALUE;
	case PieceType::Rook: return ROOK_VALUE;
	case PieceType::Queen: return QUEEN_VALUE;
	case PieceType::King: return 0; // King capture is checkmate
	default: return 0;
	}
}
