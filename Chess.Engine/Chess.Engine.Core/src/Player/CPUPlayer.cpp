/*
  ==============================================================================
	Module:			CPUPlayer
	Description:    Managing the CPU opponent player
  ==============================================================================
*/

#include "CPUPlayer.h"


CPUPlayer::CPUPlayer(std::shared_ptr<MoveGeneration> moveGeneration, std::shared_ptr<MoveEvaluation> moveEvaluation, std::shared_ptr<ChessBoard> board)
	: mMoveGeneration(moveGeneration), mMoveEvaluation(moveEvaluation), mBoard(board), mRandomGenerator(mRandomDevice())
{
}


void CPUPlayer::setCPUConfiguration(const CPUConfiguration &config)
{
	mConfig = config;
	LOG_INFO("CPU player configured:");
	LOG_INFO("\tDifficulty:\t{}", static_cast<int>(config.difficulty));
	LOG_INFO("\tPlayer:\t{}", LoggingHelper::playerColourToString(config.cpuColor).c_str());
	LOG_INFO("\tEnabled:\t{}", LoggingHelper::boolToString(config.enabled).c_str());
}


void CPUPlayer::requestMoveAsync(PlayerColor player)
{
	if (!isCPUPlayer(player))
	{
		LOG_WARNING("requestMoveAsync called for non-CPU player");
		return;
	}

	// Start async calculation
	std::thread([this, player]() { calculateMove(player); }).detach();
}


bool CPUPlayer::isCPUPlayer(PlayerColor player) const
{
	return mConfig.enabled && (player == mConfig.cpuColor);
}


void CPUPlayer::moveCalculated(PossibleMove calculatedMove)
{
	for (auto &observer : mObservers)
	{
		if (auto obs = observer.lock())
			obs->onMoveCalculated(calculatedMove);
	}
}


PossibleMove CPUPlayer::getRandomMove(const std::vector<PossibleMove> &moves)
{
	if (moves.empty())
		return {};

	std::uniform_int_distribution<size_t> distribution(0, moves.size() - 1);
	size_t								  randomIndex = distribution(mRandomGenerator);

	LOG_DEBUG("CPU selected random move {}/{}", randomIndex + 1, moves.size());
	return moves[randomIndex];
}


PossibleMove CPUPlayer::getEasyMove(const std::vector<PossibleMove> &moves)
{
	if (moves.empty())
		return {};

	// Easy: Basic move evaluation
	std::vector<MoveCandidate> evaluatedMoves;

	for (const auto &move : moves)
	{
		int score = mMoveEvaluation->getBasicEvaluation(move);
		evaluatedMoves.emplace_back(move, score);
	}

	PossibleMove selectedMove = mConfig.enableRandomization ? selectMoveWithRandomization(evaluatedMoves) : selectBestMove(evaluatedMoves);

	return selectedMove;
}


PossibleMove CPUPlayer::getMediumMove(const std::vector<PossibleMove> &moves)
{
	if (moves.empty())
		return {};

	// Medium: Enhanced evaluation with  positional awareness
	std::vector<MoveCandidate> evaluatedMoves;

	for (const auto &move : moves)
	{
		int score = mMoveEvaluation->getMediumEvaluation(move, mConfig.cpuColor);
		evaluatedMoves.emplace_back(move, score);
	}

	PossibleMove selectedMove = mConfig.enableRandomization ? selectMoveWithRandomization(evaluatedMoves) : selectBestMove(evaluatedMoves);

	return selectedMove;
}


PossibleMove CPUPlayer::getHardMove(const std::vector<PossibleMove> &moves)
{
	if (moves.empty())
		return {};

	// Hard: Advanced evaluation with deeper analysis
	std::vector<MoveCandidate> evaluatedMoves;

	for (const auto &move : moves)
	{
		int score = mMoveEvaluation->getAdvancedEvaluation(move, mConfig.cpuColor);
		evaluatedMoves.emplace_back(move, score);
	}

	PossibleMove selectedMove = mConfig.enableRandomization ? selectMoveWithRandomization(evaluatedMoves) : selectBestMove(evaluatedMoves);

	return selectedMove;
}


PossibleMove CPUPlayer::getMiniMaxMove(const std::vector<PossibleMove> &moves, int depth)
{
	if (moves.empty())
		return {};

	// Reset search statistics
	mNodesSearched	   = 0;
	mTranspositionHits = 0;

	// Create lightweight board from current board data
	LightChessBoard lightBoard(*mBoard);

	PossibleMove	bestMove  = moves[0];
	int				bestScore = -std::numeric_limits<int>::max();

	LOG_INFO("Starting minimax search with depth {}", depth);

	for (const auto &move : moves)
	{
		// make the move
		auto undoInfo = lightBoard.makeMove(move);

		// Evaluate using minimax (opp's turn -> minimizing)
		int	 score	  = minimax(lightBoard, depth - 1, false, mConfig.cpuColor);

		// unmake move
		lightBoard.unmakeMove(undoInfo);

		// update best move if this is better
		if (score > bestScore)
		{
			bestScore = score;
			bestMove  = move;
		}

		LOG_DEBUG("Move from {} to {} scored: {}", LoggingHelper::positionToString(move.start).c_str(), LoggingHelper::positionToString(move.end).c_str(), score);
	}

	LOG_INFO("Minimax search completed. Best score: {}, Nodes searched: {}", bestScore, mNodesSearched);

	return bestMove;
}


PossibleMove CPUPlayer::getAlphaBetaMove(const std::vector<PossibleMove> &moves, int depth)
{
	if (moves.empty())
		return {};

	// reset statistics
	mNodesSearched	   = 0;
	mTranspositionHits = 0;

	// create lightweight board from current board
	LightChessBoard			  lightBoard(*mBoard);

	std::vector<PossibleMove> sortedMoves = moves;

	// Sort moves for better pruning
	std::sort(sortedMoves.begin(), sortedMoves.end(),
			  [&](const PossibleMove &a, const PossibleMove &b)
			  {
				  int scoreA = 0, scoreB = 0;

				  // prioritize captures
				  if ((a.type & MoveType::Capture) == MoveType::Capture)
					  scoreA += 1000;
				  if ((b.type & MoveType::Capture) == MoveType::Capture)
					  scoreB += 1000;

				  // Prioritize checks
				  if ((a.type & MoveType::Check) == MoveType::Check)
					  scoreA += 500;
				  if ((b.type & MoveType::Check) == MoveType::Check)
					  scoreB += 500;

				  return scoreA > scoreB;
			  });

	PossibleMove bestMove  = sortedMoves[0];
	int			 bestScore = -std::numeric_limits<int>::max();
	int			 alpha	   = -std::numeric_limits<int>::max();
	int			 beta	   = std::numeric_limits<int>::max();

	LOG_INFO("Starting alpha-beta search with depth {}", depth);

	for (const auto &move : sortedMoves)
	{
		// make move
		auto undoInfo = lightBoard.makeMove(move);

		// evaluate using alpha-beta (opp's turn, so minimizing)
		int	 score	  = alphaBeta(lightBoard, depth - 1, alpha, beta, false, mConfig.cpuColor);

		// unmake move
		lightBoard.unmakeMove(undoInfo);

		// update best move if this is better
		if (score > bestScore)
		{
			bestScore = score;
			bestMove  = move;
		}

		// update alpha for pruning at root level
		alpha = std::max(alpha, score);

		LOG_DEBUG("Move from {} to {} scored: {}", LoggingHelper::positionToString(move.start).c_str(), LoggingHelper::positionToString(move.end).c_str(), score);
	}

	LOG_INFO("Alpha-Beta search completed. Best score: {}, Nodes searched: {}, Transposition hits: {}", bestScore, mNodesSearched, mTranspositionHits);

	return bestMove;
}


int CPUPlayer::evaluatePlayerPosition(const LightChessBoard &board, PlayerColor player)
{
	// TODO:	Utilize MoveEvaluation with LightChessBoard for better evaluation and improving the algorithms

	//int			score	 = 0;
	//PlayerColor opponent = (player == PlayerColor::White) ? PlayerColor::Black : PlayerColor::White;

	//// Material evaluation (from LightChessBoard)
	//score += board.getMaterialValue(player) - board.getMaterialValue(opponent);

	//// Positional evaluation
	//auto playerPieces	= board.getPiecePositions(player);
	//auto opponentPieces = board.getPiecePositions(opponent);

	//for (const auto &pos : playerPieces)
	//{
	//	auto &piece = board.getPiece(pos);

	//	if (!piece.isEmpty())
	//		score += mMoveEvaluation->getPositionValue(piece.type, pos, player);
	//}

	//for (const auto &pos : opponentPieces)
	//{
	//	auto &piece = board.getPiece(pos);

	//	if (!piece.isEmpty())
	//		score -= mMoveEvaluation->getPositionValue(piece.type, pos, opponent);
	//}

	//// King safety evaluation
	//if (!board.isEndgame())
	//{
	//	if (board.isInCheck(player))
	//		score -= 50;

	//	if (board.isInCheck(opponent))
	//		score += 50;
	//}
	//else
	//{
	//	// Endgame: encourage king activity and centralization
	//	Position playerKing			= board.getKingPosition(player);
	//	Position opponentKing		= board.getKingPosition(opponent);

	//	int		 playerCentrality	= 4 - std::max(std::abs(playerKing.x - 3.5), std::abs(playerKing.y - 3.5));
	//	int		 opponentCentrality = 4 - std::max(std::abs(opponentKing.x - 3.5), std::abs(opponentKing.y - 3.5));

	//	score += playerCentrality * 10;	  // our centralization is good
	//	score -= opponentCentrality * 10; // opponnent centralization is bad for us
	//}

	//// Mobility evaluation
	//auto playerMoves   = board.generateLegalMoves(player);
	//auto opponentMoves = board.generateLegalMoves(opponent);

	//score += static_cast<int>(playerMoves.size()) * 2;
	//score -= static_cast<int>(opponentMoves.size()) * 2;

	//return score;
	return 0;
}


void CPUPlayer::calculateMove(PlayerColor player)
{
	PossibleMove selectedMove{};

	// Generate all legal moves
	mMoveGeneration->calculateAllLegalBasicMoves(player);

	// Get all possible moves for all pieces
	auto					  playerPieces = mBoard->getPiecesFromPlayer(player);
	std::vector<PossibleMove> allMoves;

	for (const auto &[position, piece] : playerPieces)
	{
		auto moves = mMoveGeneration->getMovesForPosition(position);
		allMoves.insert(allMoves.end(), moves.begin(), moves.end());
	}

	if (allMoves.empty())
	{
		LOG_WARNING("No legal moves available for CPU player!");
		return;
	}

	// Simulate thinking
	simulateThinking();

	// Select move based on difficulty
	switch (mConfig.difficulty)
	{
	case CPUDifficulty::Random: selectedMove = getRandomMove(allMoves); break;
	case CPUDifficulty::Easy: selectedMove = getEasyMove(allMoves); break;
	case CPUDifficulty::Medium: selectedMove = getMediumMove(allMoves); break;
	case CPUDifficulty::Hard: selectedMove = getHardMove(allMoves); break;
	default: selectedMove = getRandomMove(allMoves); break;
	}

	// Set promotion piece for pawn promotion moves
	if ((selectedMove.type & MoveType::PawnPromotion) == MoveType::PawnPromotion)
	{
		selectedMove.promotionPiece = PieceType::Queen; // CPU always promotes to Queen
		LOG_INFO("CPU selected pawn promotion to Queen");
	}

	moveCalculated(selectedMove);
}


void CPUPlayer::simulateThinking()
{
	if (mConfig.thinkingTime.count() > 0)
	{
		std::this_thread::sleep_for(mConfig.thinkingTime);
	}
}


int CPUPlayer::minimax(LightChessBoard &board, int depth, bool maximizing, PlayerColor player)
{
	mNodesSearched++;

	// Terminal depth reached -> evaluate static position
	if (depth == 0)
		return evaluatePlayerPosition(board, player);

	// Generate legal moves for player
	auto moves = board.generateLegalMoves(board.getCurrentPlayer());

	// Terminal position check (checkmate/stalemate)
	if (moves.empty())
	{
		if (board.isInCheck(board.getCurrentPlayer()))
		{
			// Checkmate -> return large signed value based on perspective (prefer quicker mates by adding depth to score
			return maximizing ? (-10000 + depth) : (10000 - depth);
		}
		else
		{
			// Stalemate
			return 0;
		}
	}

	if (maximizing)
	{
		int maxEval = -std::numeric_limits<int>::max();

		for (const auto &move : moves)
		{
			// make move
			auto undoInfo = board.makeMove(move);

			// recursively evaluate (switch to minimizing player)
			int	 eval	  = minimax(board, depth - 1, false, player);
			maxEval		  = std::max(maxEval, eval);

			// Unmake move
			board.unmakeMove(undoInfo);
		}

		return maxEval;
	}
	else
	{
		int minEval = std::numeric_limits<int>::max();

		for (const auto &move : moves)
		{
			// make move
			auto undoInfo = board.makeMove(move);

			// recursively evaluate (switch to maximizing player)
			int	 eval	  = minimax(board, depth - 1, true, player);
			minEval		  = std::min(minEval, eval);

			// unmake move
			board.unmakeMove(undoInfo);
		}

		return minEval;
	}
}


int CPUPlayer::alphaBeta(LightChessBoard &board, int depth, int alpha, int beta, bool maximizing, PlayerColor player)
{
	mNodesSearched++;

	// Check transposition table first
	uint64_t	 hashKey = board.getHashKey();
	int			 storedScore{0};
	PossibleMove storedMove{};

	if (lookupTransposition(hashKey, depth, storedScore, storedMove))
	{
		mTranspositionHits++;
		return storedScore;
	}

	// Terminal depth reached -> evaluate static positon
	if (depth == 0)
	{
		int score = evaluatePlayerPosition(board, player);
		storeTransposition(hashKey, depth, score, TranspositionEntry::NodeType::Exact, PossibleMove{});
		return score;
	}

	// Generate legal mvoes for current player
	auto moves = board.generateLegalMoves(board.getCurrentPlayer());

	// Terminal positon check (checkmate/stalemate)
	if (moves.empty())
	{
		int score{0};
		if (board.isInCheck(board.getCurrentPlayer()))
		{
			// checkmate -> return large signed value based on perspective (prefer quicker mates by adding depth to score
			score = maximizing ? (-10000 + depth) : (10000 - depth);
		}
		else
		{
			// stalemate
			score = 0;
		}

		storeTransposition(hashKey, depth, score, TranspositionEntry::NodeType::Exact, PossibleMove{});
		return score;
	}

	// Move ordering : try best move from transposition table first if available
	if (!storedMove.isEmpty())
	{
		// find stored move in our moves list and put it first
		auto it = std::find_if(moves.begin(), moves.end(), [&storedMove](const PossibleMove &move) { return move == storedMove; });

		if (it != moves.end())
			std::swap(*moves.begin(), *it);
	}

	PossibleMove				 bestMove{};
	TranspositionEntry::NodeType nodeType = TranspositionEntry::NodeType::Alpha;

	if (maximizing)
	{
		int maxEval = -std::numeric_limits<int>::max();

		for (const auto &move : moves)
		{
			// make move
			auto undoInfo = board.makeMove(move);

			// recursively evaluate (switch to minimizing player)
			int	 eval	  = alphaBeta(board, depth - 1, alpha, beta, false, player);

			// unmake move
			board.unmakeMove(undoInfo);

			if (eval > maxEval)
			{
				maxEval	 = eval;
				bestMove = move;
			}

			alpha = std::max(alpha, eval);

			// alpha beta prunning
			if (beta <= alpha)
			{
				nodeType = TranspositionEntry::NodeType::Beta;
				break; // Beta cutoff
			}
		}

		// determine node type for transpotion table
		if (maxEval >= beta)
			nodeType = TranspositionEntry::NodeType::Beta;
		else if (maxEval <= alpha)
			nodeType = TranspositionEntry::NodeType::Alpha;
		else
			nodeType = TranspositionEntry::NodeType::Exact;

		// store it in transposition table
		storeTransposition(hashKey, depth, maxEval, nodeType, bestMove);

		return maxEval;
	}
	else
	{
		int minEval = std::numeric_limits<int>::max();

		for (const auto &move : moves)
		{
			// make move
			auto undoInfo = board.makeMove(move);

			// Recursively evaluate (switch to maximizing player)
			int	 eval	  = alphaBeta(board, depth - 1, alpha, beta, true, player);

			// unmake move
			board.unmakeMove(undoInfo);

			if (eval < minEval)
			{
				minEval	 = eval;
				bestMove = move;
			}

			beta = std::min(beta, eval);

			// alpha beta prunning
			if (beta <= alpha)
			{
				nodeType = TranspositionEntry::NodeType::Alpha;
				break; // alpha cutoff
			}
		}

		// Determine node type for transposition table
		if (minEval <= alpha)
			nodeType = TranspositionEntry::NodeType::Alpha;
		else if (minEval >= beta)
			nodeType = TranspositionEntry::NodeType::Beta;
		else
			nodeType = TranspositionEntry::NodeType::Exact;

		// store transposition table
		storeTransposition(hashKey, depth, minEval, nodeType, bestMove);

		return minEval;
	}
}


PossibleMove CPUPlayer::selectBestMove(std::vector<MoveCandidate> &moves)
{
	auto topCandidates = filterTopCandidates(moves);

	if (topCandidates.empty())
		return {};

	return topCandidates[0].move;
}


PossibleMove CPUPlayer::selectMoveWithRandomization(std::vector<MoveCandidate> &moves)
{
	auto topCandidates = filterTopCandidates(moves);

	if (topCandidates.empty())
		return {};

	if (topCandidates.size() == 1 || !mConfig.enableRandomization)
		return topCandidates[0].move;

	// weigh moves depending on score difference to the best move and randomization factor
	int											bestScore = topCandidates[0].score;
	std::vector<std::pair<PossibleMove, float>> weightedMoves;

	for (const auto &candidate : topCandidates)
	{
		float scoreDiff = static_cast<float>(bestScore - candidate.score);
		float weight	= std::exp(-scoreDiff * mConfig.randomizationFactor);
		weightedMoves.emplace_back(candidate.move, weight);
	}

	// Select best on weight
	std::uniform_real_distribution<float> dist(0.0f, 1.0f);
	float								  randomValue = dist(mRandomGenerator);
	float								  totalWeight = 0.0f;

	for (const auto &[move, weight] : weightedMoves)
	{
		totalWeight += weight;
	}

	float accumulatedWeight = 0.0f;
	for (const auto &[move, weight] : weightedMoves)
	{
		accumulatedWeight += weight / totalWeight;
		if (randomValue <= accumulatedWeight)
			return move;
	}

	return topCandidates[0].move;
}


std::vector<MoveCandidate> CPUPlayer::filterTopCandidates(std::vector<MoveCandidate> &allMoves)
{
	std::vector<MoveCandidate> topCandidates;
	topCandidates.reserve(mConfig.candidateMoveCount);

	// Sort by score (descending order)
	std::sort(allMoves.begin(), allMoves.end(), [](const auto &a, const auto &b) { return a.score > b.score; });

	int actualCount = (std::min)(mConfig.candidateMoveCount, static_cast<int>(allMoves.size()));

	for (int i = 0; i < actualCount; ++i)
	{
		topCandidates.push_back(allMoves[i]);
	}

	return topCandidates;
}


void CPUPlayer::storeTransposition(uint64_t hash, int depth, int score, TranspositionEntry::NodeType type, const PossibleMove &move)
{
	if (mTranspositionTable.size() >= MAX_TRANSPOSITION_ENTRIES)
	{
		mTranspositionTable.clear();
	}

	TranspositionEntry entry;
	entry.hash				  = hash;
	entry.depth				  = depth;
	entry.score				  = score;
	entry.type				  = type;
	entry.move				  = move;

	mTranspositionTable[hash] = entry;
}


bool CPUPlayer::lookupTransposition(uint64_t hash, int depth, int &score, PossibleMove &move)
{
	auto it = mTranspositionTable.find(hash);

	if (it != mTranspositionTable.end() && it->second.depth >= depth)
	{
		score = it->second.score;
		move  = it->second.move;
		return true;
	}

	return false;
}
