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


int MoveEvaluation::getMediumEvaluation(const PossibleMove &move, PlayerColor player)
{
	int score = getBasicEvaluation(move);

	score += evaluateMaterialGain(move);		   // Material evaluation
	score += evaluatePositionalGain(move, player); // Positional evaluation
	score += evaluateCenterControl(move, player);  // Center COntrol
	score += evaluateKingSafety(move, player);	   // King Safety
	score += evaluatePieceActivity(move, player);  // Piece activity

	return score;
}


int MoveEvaluation::getAdvancedEvaluation(const PossibleMove &move, PlayerColor player)
{
	int score = getMediumEvaluation(move, player);

	score += getTacticalEvaluation(move, player);	  // Advanced technical evaluation
	score += getStrategicEvaluation(move, player);	  // Advanced strategic evaluation
	score += evaluateThreatLevel(move, player);		  // Threat analysis
	score += evaluateDefensivePatterns(move, player); // Defensive patterns

	return score;
}


int MoveEvaluation::getPositionValue(PieceType piece, const Position &pos, PlayerColor player)
{
	// Ensure position is valid
	if (!pos.isValid())
		return 0;

	int row = pos.y;
	int col = pos.x;

	// For black pieces: flip the table
	if (player == PlayerColor::Black)
		row = 7 - row;

	GamePhase phase = determineGamePhase();

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


int MoveEvaluation::evaluateMaterialGain(const PossibleMove &move)
{
	if ((move.type & MoveType::Capture) != MoveType::Capture)
		return 0;

	auto &capturedPiece = mBoard->getPiece(move.end);

	if (!capturedPiece)
		return 0;

	int	  capturedValue = getPieceValue(capturedPiece->getType());

	// Consider exchanging pieces -> evaluate the trade
	auto &movingPiece	= mBoard->getPiece(move.start);
	if (movingPiece)
	{
		int movingValue = getPieceValue(movingPiece->getType());

		// Bonus for good exchanges
		return capturedValue + (::std::max)(0, (capturedValue - movingValue));
	}

	return capturedValue;
}


int MoveEvaluation::evaluatePositionalGain(const PossibleMove &move, PlayerColor player)
{
	int	  positionalScore = 0;

	auto &movingPiece	  = mBoard->getPiece(move.start);

	if (!movingPiece)
		return 0;

	PieceType piece = movingPiece->getType();

	// Add positional value for destination
	positionalScore += getPositionValue(piece, move.end, player);

	// Subtract position value for leaving current square
	positionalScore -= getPositionValue(piece, move.start, player);

	return positionalScore;
}


int MoveEvaluation::evaluateThreatLevel(const PossibleMove &move, PlayerColor player)
{
	int			score			= 0;

	// Check if this move creates threat to the opponent pieces
	auto		attackedSquares = getAttackedSquares(move.end, player);
	PlayerColor opponent		= getOpponnentColor(player);

	for (const auto &square : attackedSquares)
	{
		auto &piece = mBoard->getPiece(square);

		if (!piece || piece->getColor() != opponent)
			continue;

		// Threatening valuable pieces gives more points
		int pieceValue = getPieceValue(piece->getType());
		score += pieceValue / 10; // Threat is worth 1/10th of piece value
	}

	// Check if this moves blocks opponnent threats
	if (blocksEnemyThreats(move, player))
		score += THREAT_BLOCK_BONUS;

	return score;
}


int MoveEvaluation::evaluateKingSafety(const PossibleMove &move, PlayerColor player)
{
	int		 score			  = 0;
	Position kingPos		  = mBoard->getKingsPosition(player);
	Position opponentKingPos = mBoard->getKingsPosition(getOpponnentColor(player));

	// Penalize moves that expose our king
	if (wouldExposeKing(move, player))
		score -= 100;

	// Reward moves that attack near the opponnents king
	if (isNearKing(move.end, opponentKingPos))
		score += 30;

	// Reward defensive moves near our king when under threat
	if (isNearKing(move.end, kingPos))
	{
		int attackerCount = countAttackers(kingPos, getOpponnentColor(player));
		if (attackerCount > 0)
			score += 25;
	}

	return score;
}


int MoveEvaluation::evaluateCenterControl(const PossibleMove &move, PlayerColor player)
{
	int score = 0;

	// Reward moves that control/occupy center squares
	if (isInCenter(move.end))
		score += CENTER_CONTROL_BONUS;

	// Additional: bonus for pieces attacking center squares
	auto attackedSquares = getAttackedSquares(move.end, player);

	for (const auto &square : attackedSquares)
	{
		if (isInCenter(square))
			score += CENTER_CONTROL_BONUS / 2;
	}

	return score;
}


int MoveEvaluation::evaluatePawnStructure(const PossibleMove &move, PlayerColor player)
{
	int	  score		  = 0;
	auto &movingPiece = mBoard->getPiece(move.start);

	if (!movingPiece || movingPiece->getType() != PieceType::Pawn)
		return 0;

	// Check for pawn structure improvements
	if (isPasssedPawn(move.end, player))
		score += 50;

	if (isIsolatedPawn(move.end, player))
		score -= 20;

	if (isDoublePawn(move.end, player))
		score -= 15;

	return score;
}


int MoveEvaluation::evaluatePieceActivity(const PossibleMove &move, PlayerColor player)
{
	auto &piece = mBoard->getPiece(move.start);

	if (!piece)
		return 0;

	// Count squares piece can attack from new position
	auto attackedSquares = getAttackedSquares(move.end, player);
	int	 mobility		 = static_cast<int>(attackedSquares.size());

	// Reward increased mobility
	return mobility * 2;
}


int MoveEvaluation::evaluateDefensivePatterns(const PossibleMove &move, PlayerColor player)
{
	int		 score	 = 0;

	// Check if move defends important pieces or squares
	Position kingPos = mBoard->getKingsPosition(player);

	// Defending the king area
	if (isNearKing(move.end, kingPos))
		score += 15;

	// Defending important central squares
	if (isInCenter(move.end))
		score += 10;

	// Check if move blocks enemy attacks on our pieces
	auto attackedSquares = getAttackedSquares(move.end, player);
	for (const auto &square : attackedSquares)
	{
		auto &piece = mBoard->getPiece(square);

		if (piece && piece->getColor() == player)
			score += 5;
	}

	return score;
}


bool MoveEvaluation::createsPin(const PossibleMove &move, PlayerColor player)
{
	auto &movingPiece = mBoard->getPiece(move.start);
	if (!movingPiece)
		return false;

	// Only long range pieces (rook, bishop and queen) can create pins
	PieceType type = movingPiece->getType();
	if (type != PieceType::Bishop && type != PieceType::Rook && type != PieceType::Queen)
		return false;

	// to simulate the move we need to create a copy of the chessboard and other modules
	ChessBoard tmpBoard(*mBoard);
	tmpBoard.movePiece(move.start, move.end);

	auto		tmpValidation = std::make_shared<MoveValidation>(std::make_shared<ChessBoard>(tmpBoard));
	auto		tmpExecution  = std::make_shared<MoveExecution>(std::make_shared<ChessBoard>(tmpBoard), tmpValidation);
	auto		tmpGeneration = std::make_shared<MoveGeneration>(std::make_shared<ChessBoard>(tmpBoard), tmpValidation, tmpExecution);

	PlayerColor opponnent	  = getOpponnentColor(player);
	Position	opponnentKing = tmpBoard.getKingsPosition(opponnent);

	// Get attacked squares from the piece's new position
	tmpGeneration->calculateAllLegalBasicMoves(player);
	auto attackedSquares = tmpGeneration->getMovesForPosition(move.end);

	// Check if there is exactly one opponnent's piece between us and their king

	// Determine direction from our piece to opponnent's king
	int	 dx				 = opponnentKing.x - move.end.x;
	int	 dy				 = opponnentKing.y - move.end.y;

	// Normalize direction for sliding pieces
	int	 stepX = 0, stepY = 0;
	if (dx != 0)
		stepX = dx / abs(dx);

	if (dy != 0)
		stepY = dy / abs(dy);

	// Count pieces in between
	int		 piecesInBetween = 0;

	Position current		 = move.end;
	current.x += stepX;
	current.y += stepY;

	while (current.isValid() && current != opponnentKing)
	{
		auto &piece = tmpBoard.getPiece(current);

		if (piece && piece->getColor() == opponnent)
			piecesInBetween++;

		current.x += stepX;
		current.y += stepY;
	}

	// Pin exists if there is excactly one (opponnent's) piece between us and the king
	return piecesInBetween == 1;
}


bool MoveEvaluation::createsFork(const PossibleMove &move, PlayerColor player)
{
	int			valuableTargets	= 0;

	auto		attackedSquares = getAttackedSquares(move.end, player);
	PlayerColor opponnent		= getOpponnentColor(player);

	for (const auto &square : attackedSquares)
	{
		auto &piece = mBoard->getPiece(square);

		if (!piece || piece->getColor() != opponnent)
			continue;

		PieceType type = piece->getType();

		// Count valuable pieces
		if (type == PieceType::Knight || type == PieceType::Bishop || type == PieceType::Rook || type == PieceType::Queen || type == PieceType::King)
		{
			valuableTargets++;
		}
	}

	return valuableTargets >= 2; // Fork if attacking 2+ valuable pieces
}


bool MoveEvaluation::createsSkewer(const PossibleMove &move, PlayerColor player)
{
	auto &movingPiece = mBoard->getPiece(move.start);
	if (!movingPiece)
		return false;

	// Only long range pieces can create skewers
	PieceType type = movingPiece->getType();
	if (type != PieceType::Bishop && type != PieceType::Rook && type != PieceType::Queen)
		return false;

	// to simulate the move we need to create a copy of the chessboard and other modules
	ChessBoard tmpBoard(*mBoard);
	tmpBoard.movePiece(move.start, move.end);

	auto		tmpValidation = std::make_shared<MoveValidation>(std::make_shared<ChessBoard>(tmpBoard));
	auto		tmpExecution  = std::make_shared<MoveExecution>(std::make_shared<ChessBoard>(tmpBoard), tmpValidation);
	auto		tmpGeneration = std::make_shared<MoveGeneration>(std::make_shared<ChessBoard>(tmpBoard), tmpValidation, tmpExecution);

	PlayerColor opponent	  = getOpponnentColor(player);

	// Get attacked squares from the piece's new pos
	tmpGeneration->calculateAllLegalBasicMoves(player);
	auto allMoves = tmpGeneration->getMovesForPosition(move.end);

	// Check each direction for potential skewers
	for (const auto &attackMove : allMoves)
	{
		auto &targetPiece = tmpBoard.getPiece(attackMove.end);

		if (!targetPiece || targetPiece->getColor() != opponent)
			continue;

		// We found an opponnents piece we could attack
		// Check if there is a more valuable piece behind in the same line

		// Determine direction
		int dx	  = attackMove.end.x - move.end.x;
		int dy	  = attackMove.end.y - move.end.y;

		// Normalize direction
		int stepX = 0, stepY = 0;
		if (dx != 0)
			stepX = dx / abs(dx);
		if (dy != 0)
			stepY = dy / abs(dy);

		// Continue in the same direction to find 2nd piece
		Position current = attackMove.end;
		current.x += stepX;
		current.y += stepY;

		while (current.isValid())
		{
			auto &secondPiece = tmpBoard.getPiece(current);

			if (secondPiece)
			{
				// Found 2nd piece
				if (secondPiece->getColor() != opponent)
					break; // found our piece, stop looking in this direction

				int firstPieceValue	 = getPieceValue(targetPiece->getType());
				int secondPieceValue = getPieceValue(secondPiece->getType());

				// Skewer: first piece is more valuable and will be forced to move exposing 2nd piece
				if (firstPieceValue > secondPieceValue)
					return true;
			}

			current.x += stepX;
			current.y += stepY;
		}
	}

	return false;
}


bool MoveEvaluation::blocksEnemyThreats(const PossibleMove &move, PlayerColor player)
{
	// We calculate the threats before and after the move to compare the two

	Position			  ourKing	= mBoard->getKingsPosition(player);
	PlayerColor			  opponent = getOpponnentColor(player);
	std::vector<Position> currentThreats;
	std::vector<Position> threatsAfterMove;

	// Cound current threads against our king and important pieces
	mGeneration->calculateAllLegalBasicMoves(opponent);

	auto opponnentPieces = mBoard->getPiecesFromPlayer(opponent);

	for (const auto &[pos, piece] : opponnentPieces)
	{
		auto opponnentMoves = mGeneration->getMovesForPosition(pos);
		for (const auto &opponnentMove : opponnentMoves)
		{
			// Check if opponnent is threatening our pieces
			auto &threateningPiece = mBoard->getPiece(opponnentMove.end);
			if (threateningPiece && threateningPiece->getColor() == player)
			{
				currentThreats.push_back(opponnentMove.end);
			}
		}
	}

	// Simulate the move on copies of the modules
	ChessBoard tmpBoard(*mBoard);
	tmpBoard.movePiece(move.start, move.end);

	// Create temporary MoveGeneration for the board after our move
	auto	 tmpValidation	  = std::make_shared<MoveValidation>(std::make_shared<ChessBoard>(tmpBoard));
	auto	 tmpExecution	  = std::make_shared<MoveExecution>(std::make_shared<ChessBoard>(tmpBoard), tmpValidation);
	auto	 tmpGeneration	  = std::make_shared<MoveGeneration>(std::make_shared<ChessBoard>(tmpBoard), tmpValidation, tmpExecution);

	// Update king position if king was moved
	Position ourKingAfterMove = tmpBoard.getKingsPosition(player);

	// Count threats after move
	tmpGeneration->calculateAllLegalBasicMoves(opponent);
	auto opponnentsPiecesAfterMove = tmpBoard.getPiecesFromPlayer(opponent);

	for (const auto &[pos, piece] : opponnentsPiecesAfterMove)
	{
		auto opponentMoves = tmpGeneration->getMovesForPosition(pos);
		for (const auto &opponentMove : opponentMoves)
		{
			// Check if opponent is threatening our pieces
			auto &threatenedPiece = tmpBoard.getPiece(opponentMove.end);
			if (threatenedPiece && threatenedPiece->getColor() == player)
			{
				threatsAfterMove.push_back(opponentMove.end);
			}
		}
	}

	// We blocked enemy threats if:
	// 1. We reduced threats against our king
	// 2. We reduced total threats against our pieces
	// 3. Our move physically blocks a line of attack

	// Check if we reduced the number of threats (especially against our king)
	bool blockedKingThreats = false;
	int	 kingThreatsBefore	= 0;
	int	 kingThreatsAfter	= 0;

	for (const auto &threat : currentThreats)
	{
		if (threat == ourKing)
			kingThreatsBefore++;
	}

	for (const auto &threat : threatsAfterMove)
	{
		if (threat == ourKingAfterMove)
			kingThreatsAfter++;
	}

	blockedKingThreats		 = (kingThreatsAfter < kingThreatsBefore);
	bool reducedTotalThreats = (threatsAfterMove.size() < currentThreats.size());

	// Check if our move physically interposes between an enemy piece and our king
	bool physicallyBlocked	 = false;

	// Check if our destination square blocks any line of attack to our king
	for (const auto &[enemyPos, piece] : opponnentsPiecesAfterMove)
	{
		PieceType enemyType = piece->getType();

		// Only check long range pieces
		if (enemyType == PieceType::Bishop || enemyType == PieceType::Rook || enemyType == PieceType::Queen)
		{
			// Check if enemy piece, our move destination, and our king are collinear
			int	 dx1	   = move.end.x - enemyPos.x;
			int	 dy1	   = move.end.y - enemyPos.y;
			int	 dx2	   = ourKingAfterMove.x - enemyPos.x;
			int	 dy2	   = ourKingAfterMove.y - enemyPos.y;

			// Check if they're on the same line
			bool collinear = false;

			if (enemyType == PieceType::Rook || enemyType == PieceType::Queen)
			{
				collinear = (dx1 == 0 && dx2 == 0) || (dy1 == 0 && dy2 == 0);
			}
			if ((enemyType == PieceType::Bishop || enemyType == PieceType::Queen) && !collinear)
			{
				collinear = (dx1 != 0 && dy1 != 0 && dx2 != 0 && dy2 != 0 && abs(dx1) * dy2 == abs(dy1) * dx2 && ((dx1 > 0) == (dx2 > 0)) && ((dy1 > 0) == (dy2 > 0)));
			}

			if (collinear)
			{
				// Check if our piece is in between the enemy and our king
				int distToEnemy = abs(dx1) + abs(dy1);
				int distToKing	= abs(dx2) + abs(dy2);

				if (distToEnemy < distToKing)
				{
					physicallyBlocked = true;
					break;
				}
			}
		}
	}

	return blockedKingThreats || reducedTotalThreats || physicallyBlocked;
}


int MoveEvaluation::getStrategicEvaluation(const PossibleMove &move, PlayerColor player)
{
	int score = 0;

	score += evaluatePawnStructure(move, player); // Pawn structure considerations

	// King's safety considerations (weighed more in middlegame)
	GamePhase phase = determineGamePhase();

	if (phase != GamePhase::EndGame)
		score += evaluateKingSafety(move, player) * KING_SAFETY_WEIGHT;

	score += evaluatePieceActivity(move, player); // Piece coordination and activity

	return score;
}


int MoveEvaluation::getTacticalEvaluation(const PossibleMove &move, PlayerColor player)
{
	int score = 0;

	if (createsFork(move, player))
		score += FORK_BONUS;

	if (createsPin(move, player))
		score += PIN_BONUS;

	if (createsSkewer(move, player))
		score += SKEWER_BONUS;

	score += evaluateThreatLevel(move, player) * THREAT_WEIGHT;

	return score;
}


GamePhase MoveEvaluation::determineGamePhase() const
{
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
	if (totalMaterial > 6000 || totalPieces > 20)
		return GamePhase::Opening;

	else if (totalMaterial > 2500 || totalPieces > 12)
		return GamePhase::MiddleGame;

	else
		return GamePhase::EndGame;
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
	PlayerColor opponent = getOpponnentColor(player);
	int			attackers = countAttackers(kingPos, opponent);
	score -= 20 * attackers; // Penalty for each attacker

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
				score += 5; // Bonus for friendly pieces near king
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
			score += 50;

		if (isIsolatedPawn(position, player))
			score -= 20;

		if (isDoublePawn(position, player))
			score -= 15;
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


std::vector<Position> MoveEvaluation::getAttackedSquares(const Position &piecePos, PlayerColor player) const
{
	std::vector<Position> attackedSquares;

	mGeneration->calculateAllLegalBasicMoves(player);

	auto possibleMoves = mGeneration->getMovesForPosition(piecePos);

	for (const auto &move : possibleMoves)
	{
		attackedSquares.push_back(move.end);
	}

	return attackedSquares;
}


bool MoveEvaluation::wouldExposeKing(const PossibleMove &move, PlayerColor player) const
{
	Position kingPos = mBoard->getKingsPosition(player);

	// If we're moving a piece from near the king, check if it exposes the king
	if (isNearKing(move.start, kingPos) && !isNearKing(move.end, kingPos))
	{
		// Count current attackers
		int currentAttackers = countAttackers(kingPos, getOpponnentColor(player));
		return currentAttackers > 0; // Risk if already under attack
	}

	return false;
}


int MoveEvaluation::countAttackers(const Position &target, PlayerColor attackerPlayer) const
{
	int count = 0;

	for (int y = 0; y < BOARD_SIZE; ++y)
	{
		for (int x = 0; x < BOARD_SIZE; ++x)
		{
			Position pos{x, y};
			auto	&piece = mBoard->getPiece(pos);

			if (!piece || piece->getColor() != attackerPlayer)
				continue;

			auto attackedSquares = getAttackedSquares(pos, attackerPlayer);

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


PlayerColor MoveEvaluation::getOpponnentColor(PlayerColor player) const
{
	return (player == PlayerColor::White) ? PlayerColor::Black : PlayerColor::White;
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
