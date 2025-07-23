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

	LOG_INFO("CPU selected random move {}/{}", randomIndex + 1, moves.size());
	return moves[randomIndex];
}


PossibleMove CPUPlayer::getEasyMove(const std::vector<PossibleMove> &moves)
{
	// Easy: Basic move evaluation
	std::vector<std::pair<PossibleMove, int>> evaluatedMoves;

	for (const auto &move : moves)
	{
		int score = mMoveEvaluation->getBasicEvaluation(move);
		evaluatedMoves.emplace_back(move, score);
	}

	// Sort by score (descending order)
	std::sort(evaluatedMoves.begin(), evaluatedMoves.end(), [](const auto &a, const auto &b) { return a.second > b.second; });

	// Pick from top 3 moves with somewhat randomness
	size_t								  topMoves = (evaluatedMoves.size() < 3) ? evaluatedMoves.size() : 3;
	std::uniform_int_distribution<size_t> distribution(0, topMoves - 1);
	size_t								  selectedIndex = distribution(mRandomGenerator);

	return evaluatedMoves[selectedIndex].first;
}


PossibleMove CPUPlayer::getMediumMove(const std::vector<PossibleMove> &moves)
{
	// TODO
	return PossibleMove();
}


PossibleMove CPUPlayer::getHardMove(const std::vector<PossibleMove> &moves)
{
	// TODO
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

	// Select move based on difficulty (for now we just test random moves)
	switch (mConfig.difficulty)
	{
	case CPUDifficulty::Random: selectedMove = getRandomMove(allMoves); break;
	case CPUDifficulty::Easy: selectedMove = getEasyMove(allMoves); break;
	case CPUDifficulty::Medium: selectedMove = getMediumMove(allMoves); break;
	case CPUDifficulty::Hard: selectedMove = getHardMove(allMoves); break;
	default: selectedMove = getRandomMove(allMoves); break;
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
