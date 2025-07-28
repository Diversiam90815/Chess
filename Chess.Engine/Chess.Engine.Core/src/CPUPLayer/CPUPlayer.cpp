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
	std::uniform_int_distribution<size_t> distribution(0, moves.size() - 1);
	size_t								  randomIndex = distribution(mRandomGenerator);

	LOG_DEBUG("CPU selected random move {}/{}", randomIndex + 1, moves.size());
	return moves[randomIndex];
}


PossibleMove CPUPlayer::getEasyMove(const std::vector<PossibleMove> &moves)
{
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
	return PossibleMove();
}

PossibleMove CPUPlayer::getAlphaBetaMove(const std::vector<PossibleMove> &moves, int depth)
{
	return PossibleMove();
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
	return 0;
}


int CPUPlayer::alphaBeta(LightChessBoard &board, int depth, int alpha, int beta, bool maximizing, PlayerColor player)
{
	return 0;
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
