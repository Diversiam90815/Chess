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
	mPositionalEvaluation = std::make_shared<PositionalEvaluation>(mMoveEvaluation);
}

CPUPlayer::~CPUPlayer()
{
	if (mSearchThread.joinable())
	{
		mSearchThread.request_stop();
		mSearchThread.join();
	}
}


void CPUPlayer::setCPUConfiguration(const CPUConfiguration &config)
{
	mConfig = config;
	LOG_INFO("CPU player configured:");
	LOG_INFO("\tDifficulty:\t{}", static_cast<int>(config.difficulty));
	LOG_INFO("\tPlayer:\t{}", LoggingHelper::playerColourToString(config.cpuColor).c_str());
	LOG_INFO("\tEnabled:\t{}", LoggingHelper::boolToString(config.enabled).c_str());
}


void CPUPlayer::requestMoveAsync()
{
	// Start async calculation
	if (!mConfig.enabled)
		return;

	launchSearchAsync(mConfig.cpuColor);
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


PossibleMove CPUPlayer::getBestEvaluatedMove(const std::vector<PossibleMove> &moves)
{
	if (moves.empty())
		return {};

	std::vector<MoveCandidate> evaluatedMoves;

	for (const auto &move : moves)
	{
		int score = mMoveEvaluation->getAdvancedEvaluation(move, mConfig.cpuColor);
		evaluatedMoves.emplace_back(move, score);
	}

	PossibleMove selectedMove = mConfig.enableRandomization ? selectMoveWithRandomization(evaluatedMoves) : selectBestMove(evaluatedMoves);

	return selectedMove;
}


PossibleMove CPUPlayer::getMiniMaxMove(const std::vector<PossibleMove> &moves, int depth, std::stop_token stopToken)
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
		if (cancelled(stopToken))
			break;

		// make the move
		auto undoInfo = lightBoard.makeMove(move);

		// Evaluate using minimax (opp's turn -> minimizing)
		int	 score	  = minimax(move, lightBoard, depth - 1, false, mConfig.cpuColor);

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


PossibleMove CPUPlayer::getAlphaBetaMove(const std::vector<PossibleMove> &moves, int depth, std::stop_token stopToken)
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
				  int scoreA = mMoveEvaluation->getAdvancedEvaluation(a, mConfig.cpuColor, &lightBoard);
				  int scoreB = mMoveEvaluation->getAdvancedEvaluation(b, mConfig.cpuColor, &lightBoard);
				  return scoreA > scoreB;
			  });

	PossibleMove bestMove  = sortedMoves[0];
	int			 bestScore = -std::numeric_limits<int>::max();
	int			 alpha	   = -std::numeric_limits<int>::max();
	int			 beta	   = std::numeric_limits<int>::max();

	LOG_INFO("Starting alpha-beta search with depth {}", depth);

	for (const auto &move : sortedMoves)
	{
		if (cancelled(stopToken)) // Check if thread was asked to cancel operation
			break;

		// make move
		auto undoInfo = lightBoard.makeMove(move);

		// evaluate using alpha-beta (opp's turn, so minimizing)
		int	 score	  = alphaBeta(move, lightBoard, depth - 1, alpha, beta, false, mConfig.cpuColor, stopToken);

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
	uint64_t hash = board.getHashKey();

	// Check evaluation cache
	auto	 it	  = mEvaluationCache.find(hash);
	if (it != mEvaluationCache.end())
		return it->second;

	int score = mPositionalEvaluation->evaluatePosition(board, player);

	// Cache the result
	if (mEvaluationCache.size() < MAX_EVAL_CACHE_SIZE)
		mEvaluationCache[hash] = score;

	return score;
}


PossibleMove CPUPlayer::computeBestMove(PlayerColor player, std::stop_token stopToken)
{
	PossibleMove selectedMove{};

	// Generate all legal moves
	mMoveGeneration->calculateAllLegalBasicMoves(player);

	if (cancelled(stopToken)) // Check if thread was asked to cancel operation
		return {};

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
		return {};
	}

	// Clear evaluation cache before starting a new search
	mEvaluationCache.clear();

// Add debugging to see move evaluation spread
#if DEBUG_MOVES

	LOG_DEBUG("=== Move Evaluation Debug ===");
	for (const auto &move : allMoves)
	{
		LightChessBoard testBoard(*mBoard);
		int				score = evaluateMoveAndPosition(move, player, testBoard);
		LOG_DEBUG("Move {}->{}: score = {}", LoggingHelper::positionToString(move.start).c_str(), LoggingHelper::positionToString(move.end).c_str(), score);
	}
	LOG_DEBUG("=== End Debug ===");

#endif

	if (cancelled(stopToken)) // Check if thread was asked to cancel operation
		return {};

	// Select move based on difficulty
	switch (mConfig.difficulty)
	{
	case CPUDifficulty::Random: selectedMove = getRandomMove(allMoves); break;
	case CPUDifficulty::Easy: selectedMove = (allMoves.size() > 20) ? getBestEvaluatedMove(allMoves) : getMiniMaxMove(allMoves, 3, stopToken); break;
	case CPUDifficulty::Medium: selectedMove = getAlphaBetaMove(allMoves, 3, stopToken); break;
	case CPUDifficulty::Hard: selectedMove = getAlphaBetaMove(allMoves, 6, stopToken); break;
	default: selectedMove = getRandomMove(allMoves); break;
	}

	// Set promotion piece for pawn promotion moves
	if ((selectedMove.type & MoveType::PawnPromotion) == MoveType::PawnPromotion)
	{
		selectedMove.promotionPiece = PieceType::Queen; // CPU always promotes to Queen
		LOG_INFO("CPU selected pawn promotion to Queen");
	}

	return cancelled(stopToken) ? PossibleMove{} : selectedMove;
}


int CPUPlayer::minimax(const PossibleMove &move, LightChessBoard &board, int depth, bool maximizing, PlayerColor player, std::stop_token stopToken)
{
	if (cancelled(stopToken))
		return 0;

	mNodesSearched++;

	// Terminal depth reached -> evaluate static position
	if (depth == 0)
		return evaluateMoveAndPosition(move, player, board);

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

		for (const auto &currentMove : moves)
		{
			if (cancelled(stopToken)) // Check if thread was asked to quit operation
				break;

			// make move
			auto undoInfo = board.makeMove(currentMove);

			// recursively evaluate (switch to minimizing player)
			int	 eval	  = minimax(currentMove, board, depth - 1, false, player, stopToken);
			maxEval		  = std::max(maxEval, eval);

			// Unmake move
			board.unmakeMove(undoInfo);
		}

		return maxEval;
	}
	else
	{
		int minEval = std::numeric_limits<int>::max();

		for (const auto &currentMove : moves)
		{
			if (cancelled(stopToken)) // Check if thread was asked to quit operation
				break;

			// make move
			auto undoInfo = board.makeMove(currentMove);

			// recursively evaluate (switch to maximizing player)
			int	 eval	  = minimax(currentMove, board, depth - 1, true, player, stopToken);
			minEval		  = std::min(minEval, eval);

			// unmake move
			board.unmakeMove(undoInfo);
		}

		return minEval;
	}
}


int CPUPlayer::alphaBeta(const PossibleMove &move, LightChessBoard &board, int depth, int alpha, int beta, bool maximizing, PlayerColor player, std::stop_token stopToken)
{
	if (cancelled(stopToken))
		return 0;

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
		int score = quiescence(board, alpha, beta, player, stopToken);
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
		auto it = std::find(moves.begin(), moves.end(), storedMove);
		if (it != moves.end())
			std::swap(*moves.begin(), *it);
	}

	// Simple capture prioritization (stable)
	std::stable_sort(moves.begin(), moves.end(),
					 [](const PossibleMove &a, const PossibleMove &b)
					 {
						 bool ac = (a.type & MoveType::Capture) == MoveType::Capture;
						 bool bc = (b.type & MoveType::Capture) == MoveType::Capture;
						 return ac > bc;
					 });

	PossibleMove				 bestMove{};
	TranspositionEntry::NodeType nodeType = TranspositionEntry::NodeType::Alpha;

	if (maximizing)
	{
		int maxEval = -std::numeric_limits<int>::max();

		for (const auto &currentMove : moves)
		{
			if (cancelled(stopToken)) // Check if thread was asked to quit operation
				break;

			// make move
			auto undoInfo = board.makeMove(currentMove);

			// recursively evaluate (switch to minimizing player)
			int	 eval	  = alphaBeta(currentMove, board, depth - 1, alpha, beta, false, player, stopToken);

			// unmake move
			board.unmakeMove(undoInfo);

			if (eval > maxEval)
			{
				maxEval	 = eval;
				bestMove = currentMove;
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

		for (const auto &currentMove : moves)
		{
			// make move
			auto undoInfo = board.makeMove(currentMove);

			// Recursively evaluate (switch to maximizing player)
			int	 eval	  = alphaBeta(currentMove, board, depth - 1, alpha, beta, true, player, stopToken);

			// unmake move
			board.unmakeMove(undoInfo);

			if (eval < minEval)
			{
				minEval	 = eval;
				bestMove = currentMove;
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


int CPUPlayer::quiescence(LightChessBoard &board, int alpha, int beta, PlayerColor player, std::stop_token stopToken)
{
	if (cancelled(stopToken)) // Check if thread was asked to quit operation
		return alpha;

	// Stand-pat - just positional evaluation from players perspective
	int stand = evaluatePlayerPosition(board, player);

	if (stand >= beta)
		return stand;
	if (stand > alpha)
		alpha = stand;

	// Generate legal moves (only consider captures though)
	auto moves = board.generateLegalMoves(board.getCurrentPlayer());

	for (const auto &move : moves)
	{
		if (cancelled(stopToken)) // Check if thread was asked to quit operation
			break;

		if ((move.type & MoveType::Capture) != MoveType::Capture)
			continue;

		auto undo  = board.makeMove(move);

		int	 score = -quiescence(board, -beta, -alpha, player);
		board.unmakeMove(undo);

		if (score >= beta)
			return score;
		if (score > alpha)
			alpha = score;
	}

	return alpha;
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


std::vector<MoveCandidate> CPUPlayer::filterTopCandidates(std::vector<MoveCandidate> &allMoves) const
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


int CPUPlayer::evaluateMoveAndPosition(const PossibleMove &move, PlayerColor player, const LightChessBoard &board)
{
	uint64_t hash = makeEvalKey(move, player, board);

	// Check evaluation cache
	auto	 it	  = mEvaluationCache.find(hash);

	if (it != mEvaluationCache.end())
		return it->second;

	// Combine positional and move-specific evaluation with proper scaling
	int score			= 0;

	// Get positional evaluation (board state)
	int positionalScore = mPositionalEvaluation->evaluatePosition(board, player);

	// Get move-specific evaluation
	int moveScore		= mMoveEvaluation->getAdvancedEvaluation(move, player, &board);

	// Combine with proper weighting
	score				= positionalScore + moveScore;

#if DEBUG_MOVES
	LOG_DEBUG("Position score: {}, Move score: {}, Total: {}", positionalScore, moveScore, score);
#endif

	// Cache result
	if (mEvaluationCache.size() < MAX_EVAL_CACHE_SIZE)
		mEvaluationCache[hash] = score;

	return score;
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


void CPUPlayer::launchSearchAsync(PlayerColor player)
{
	if (mSearchThread.joinable())
	{
		mSearchThread.request_stop();
		mSearchThread.join();
	}

	mSearchThread = std::jthread(
		[this, player](std::stop_token token)
		{
			PossibleMove mv = computeBestMove(player, token);

			if (!mv.isEmpty())
			{
				moveCalculated(mv);
			}
		});
}
