/*
  ==============================================================================
	Module:			CPUPlayer
	Description:    Managing the CPU opponent player
  ==============================================================================
*/

#include "CPUPlayer.h"

constexpr int INF	  = std::numeric_limits<int>::max();
constexpr int NEG_INF = std::numeric_limits<int>::min() + 1;


CPUPlayer::CPUPlayer(GameEngine &engine) : mEngine(engine), mRandomGenerator(mRandomDevice()) {}


CPUPlayer::~CPUPlayer()
{
	cancelCalculation();
}


void CPUPlayer::configure(const CPUConfiguration &config)
{
	mConfig = config;
	LOG_INFO("CPU player configured:");
	LOG_INFO("\tDifficulty:\t{}", static_cast<int>(config.difficulty));
	LOG_INFO("\tPlayer:\t{}", LoggingHelper::sideToString(config.cpuColor).c_str());
	LOG_INFO("\tEnabled:\t{}", LoggingHelper::boolToString(config.enabled).c_str());

	clearTranspositionTable();
}


void CPUPlayer::calculateMoveAsync(std::function<void(Move)> callback)
{
	cancelCalculation();

	mIsCalculating.store(true);

	mSearchThread = std::jthread(
		[this, callback](std::stop_token stopToken)
		{
			Move bestMove = computeBestMove(stopToken);
			mIsCalculating.store(false);

			if (!stopToken.stop_requested() && callback)
				callback(bestMove);
		});
}


Move CPUPlayer::calculateMove()
{
	std::stop_source stopSource;
	return computeBestMove(stopSource.get_token());
}


void CPUPlayer::cancelCalculation()
{
	if (mSearchThread.joinable())
	{
		mSearchThread.request_stop();
		mSearchThread.join();
	}
	mIsCalculating.store(false);
}


Move CPUPlayer::computeBestMove(std::stop_token stopToken)
{
	MoveList legalMoves;
	mEngine.generateLegalMoves(legalMoves);

	if (legalMoves.size() == 0)
	{
		LOG_WARNING("CPU has no legal moves");
		return Move();
	}

	if (legalMoves.size() == 1)
	{
		LOG_INFO("CPU has only one legal move.");
		return legalMoves[0];
	}

	mNodesSearched	   = 0;
	mTranspositionHits = 0;
	Move bestMove;

	bestMove = searchAlphaBeta(legalMoves, getSearchDepth(), stopToken);

	LOG_INFO("CPU searched {} nodes, {} TranspositionHits", mNodesSearched, mTranspositionHits);

	return bestMove;
}


Move CPUPlayer::searchAlphaBeta(const MoveList &moves, int depth, std::stop_token stopToken)
{
	std::vector<ScoredMove> scoredMoves;
	scoredMoves.reserve(moves.size());

	int alpha = NEG_INF;
	int beta  = INF;

	for (size_t i = 0; i < moves.size(); ++i)
	{
		if (isCancelled(stopToken))
			break;

		Move move = moves[i];

		if (!mEngine.makeMove(move))
			continue;

		int score = -alphaBeta(depth - 1, -beta, -alpha, false, stopToken);
		mEngine.undoMove();

		scoredMoves.push_back({move, score});

		if (score > alpha)
			alpha = score;
	}

	if (scoredMoves.empty())
		return Move();

	if (mConfig.enableRandomization)
		return selectWithRandomization(scoredMoves);

	return selectBestMove(scoredMoves);
}


int CPUPlayer::alphaBeta(int depth, int alpha, int beta, bool maximizing, std::stop_token stopToken)
{
	if (isCancelled(stopToken))
		return 0;

	++mNodesSearched;

	// Check transposition table
	uint64_t hash = mEngine.getHash();
	int		 ttScore;
	Move	 ttMove;

	if (lookupTransposition(hash, depth, alpha, beta, ttScore, ttMove))
	{
		++mTranspositionHits;
		return ttScore;
	}

	if (depth <= 0)
		return quiescence(alpha, beta, stopToken);

	MoveList moves;
	mEngine.generateLegalMoves(moves);

	// Checkmate/Stalemate
	if (moves.size() == 0)
	{
		if (mEngine.isInCheck())
			return NEG_INF - ((mConfig.maxDepth - depth)); // prefer faster checkmate

		return 0;										   // stalemate
	}

	Move						 bestMove;
	TranspositionEntry::NodeType nodeType = TranspositionEntry::NodeType::UpperBound;

	for (size_t i = 0; i < moves.size(); ++i)
	{
		if (isCancelled(stopToken))
			break;

		Move move = moves[i];

		if (!mEngine.makeMove(move))
			continue;

		int score = -alphaBeta(depth - 1, -beta, -alpha, !maximizing, stopToken);
		mEngine.undoMove();

		if (score >= beta)
		{
			storeTransposition(hash, depth, beta, TranspositionEntry::NodeType::LowerBound, move);
			return beta; // beta cutoff
		}
		if (score > alpha)
		{
			alpha	 = score;
			bestMove = move;
			nodeType = TranspositionEntry::NodeType::Exact;
		}
	}

	storeTransposition(hash, depth, alpha, nodeType, bestMove);

	return alpha;
}


int CPUPlayer::quiescence(int alpha, int beta, std::stop_token stopToken)
{
	if (isCancelled(stopToken))
		return 0;

	++mNodesSearched;

	int standPat = evaluate();

	if (standPat >= beta)
		return beta;

	if (standPat > alpha)
		alpha = standPat;

	// generate only capture moves
	MoveList moves;
	mEngine.generateLegalMoves(moves);

	for (size_t i = 0; i < moves.size(); ++i)
	{
		Move move = moves[i];

		if (!move.isCapture())
			continue;

		if (!mEngine.makeMove(move))
			continue;

		int score = -quiescence(-beta, -alpha, stopToken);
		mEngine.undoMove();

		if (score >= beta)
			return beta;

		if (score > alpha)
			alpha = score;
	}

	return alpha;
}


Move CPUPlayer::selectBestMove(std::vector<ScoredMove> &scoredMoves)
{
	if (scoredMoves.empty())
		return Move();

	auto best = std::max_element(scoredMoves.begin(), scoredMoves.end());

	return best->move;
}


Move CPUPlayer::selectWithRandomization(std::vector<ScoredMove> &scoredMoves)
{
	if (scoredMoves.empty())
		return Move();

	// sort descending by score
	std::sort(scoredMoves.begin(), scoredMoves.end());

	auto topMoves = filterTopCandidates(scoredMoves, 5);

	if (topMoves.empty())
		return scoredMoves[0].move;

	std::uniform_int_distribution<size_t> dist(0, topMoves.size() - 1);

	return topMoves[dist(mRandomGenerator)].move;
}


std::vector<ScoredMove> CPUPlayer::filterTopCandidates(std::vector<ScoredMove> &moves, size_t maxCount) const
{
	if (moves.empty())
		return {};

	int						bestScore = moves[0].score;
	int						threshold = 50;

	std::vector<ScoredMove> filtered;

	for (const auto &sm : moves)
	{
		if (filtered.size() >= maxCount)
			break;

		if (bestScore - sm.score <= threshold)
			filtered.push_back(sm);
	}

	return filtered;
}


void CPUPlayer::storeTransposition(uint64_t hash, int depth, int score, TranspositionEntry::NodeType type, Move bestMove)
{
	if (mTranspositionTable.size() >= MAX_TRANSPOSITION_ENTRIES)
		mTranspositionTable.clear(); // TODO: May need refactoring

	mTranspositionTable[hash] = {hash, depth, score, type, bestMove};
}


bool CPUPlayer::lookupTransposition(uint64_t hash, int depth, int alpha, int beta, int &score, Move &bestMove)
{
	auto it = mTranspositionTable.find(hash);
	if (it == mTranspositionTable.end())
		return false;

	const auto &entry = it->second;

	if (entry.depth < depth)
		return false;

	bestMove = entry.bestMove;

	switch (entry.type)
	{
	case TranspositionEntry::NodeType::Exact:
	{
		score = entry.score;
		return true;
	}
	case TranspositionEntry::NodeType::LowerBound:
	{
		if (entry.score >= beta)
			return true;

		break;
	}
	case TranspositionEntry::NodeType::UpperBound:
	{
		if (entry.score <= alpha)
		{
			score = entry.score;
			return true;
		}
		break;
	}
	}

	return false;
}


void CPUPlayer::clearTranspositionTable()
{
	mTranspositionTable.clear();
	mNodesSearched	   = 0;
	mTranspositionHits = 0;
}


int CPUPlayer::evaluate() const
{
	// TODO: Implement evaluation
	return 0;
}


int CPUPlayer::getSearchDepth() const
{
	switch (mConfig.difficulty)
	{
	case CPUDifficulty::Easy: return 2;
	case CPUDifficulty::Medium: return 4;
	case CPUDifficulty::Hard: return mConfig.maxDepth;
	default: return 4;
	}
}
