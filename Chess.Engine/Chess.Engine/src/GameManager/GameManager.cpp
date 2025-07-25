/*
  ==============================================================================
	Module:         GameManager
	Description:    Manager for the Chess game
  ==============================================================================
*/

#include "GameManager.h"

#include "StateMachine.h"


GameManager::~GameManager()
{
	deinitObservers();
}


GameManager *GameManager::GetInstance()
{
	static GameManager *sInstance = nullptr;
	if (nullptr == sInstance)
	{
		sInstance = new GameManager();
	}
	return sInstance;
}


void GameManager::ReleaseInstance()
{
	GameManager *sInstance = GetInstance();
	if (sInstance)
	{
		delete sInstance;
	}
}


bool GameManager::init()
{
	mLog.initLogging();
	mUserSettings.init();

	mUiCommunicationLayer = std::make_shared<UICommunication>();

	mEngine				  = std::make_shared<GameEngine>();
	mEngine->init();

	mNetwork = std::make_unique<NetworkManager>();
	mNetwork->init();

	initObservers();

	return true;
}


bool GameManager::startGame()
{
	mEngine->startGame();

	return true;
}


void GameManager::setDelegate(PFN_CALLBACK pDelegate)
{
	mUiCommunicationLayer->setDelegate(pDelegate);
}


std::vector<PossibleMove> GameManager::getPossibleMoveForPosition()
{
	return mEngine->getPossibleMoveForPosition();
}


bool GameManager::getBoardState(int boardState[BOARD_SIZE][BOARD_SIZE])
{
	return mEngine->getBoardState(boardState);
}


void GameManager::switchTurns()
{
	mEngine->switchTurns();
}


bool GameManager::calculateAllMovesForPlayer()
{
	return mEngine->calculateAllMovesForPlayer();
}


bool GameManager::initiateMove(const Position &startPosition)
{
	return mEngine->initiateMove(startPosition);
}


void GameManager::executeMove(PossibleMove &tmpMove, bool fromRemote)
{
	mEngine->executeMove(tmpMove, fromRemote);
}


void GameManager::undoMove()
{
	mEngine->undoMove();
}


void GameManager::resetGame()
{
	mEngine->resetGame();
}


std::optional<PlayerColor> GameManager::getWinner() const
{
	return mEngine->getWinner();
}


bool GameManager::checkForValidMoves(const PossibleMove &move)
{
	return mEngine->checkForValidMoves(move);
}


bool GameManager::checkForPawnPromotionMove(const PossibleMove &move)
{
	return mEngine->checkForPawnPromotionMove(move);
}


std::vector<NetworkAdapter> GameManager::getNetworkAdapters()
{
	return mNetwork->getAvailableNetworkAdapters();
}


bool GameManager::changeCurrentNetworkAdapter(int ID)
{
	return mNetwork->changeCurrentNetworkAdapter(ID);
}


int GameManager::getCurrentNetworkAdapterID()
{
	return mNetwork->getCurrentNetworkAdapterID();
}


void GameManager::setLocalPlayerName(std::string name)
{
	mPlayerName.setLocalPlayerName(name);
}


std::string GameManager::getLocalPlayerName()
{
	return mPlayerName.getLocalPlayerName();
}


EndGameState GameManager::checkForEndGameConditions()
{
	return mEngine->checkForEndGameConditions();
}


bool GameManager::startMultiplayerGame()
{
	LOG_INFO("Game started in Multiplayer mode..");

	mIsMultiplayerMode = true;

	mEngine->startGame();

	PlayerColor localPlayer = mMultiplayerManager->mLocalPlayerColor;
	mEngine->setLocalPlayer(localPlayer);

	initMultiplayerObservers();

	return true;
}


void GameManager::disconnectMultiplayerGame()
{
	if (mMultiplayerManager)
		mMultiplayerManager->disconnect();

	mIsMultiplayerMode = false;
	resetGame(); // Reset to single player
}


void GameManager::startedMultiplayer()
{
	// The player clicked on the Multiplayer menu button and started the multiplayer

	LOG_INFO("Multiplayer started..");

	mMultiplayerManager = std::make_shared<MultiplayerManager>(); // Create the multiplayer manager

	mMultiplayerManager->attachObserver(mUiCommunicationLayer);	  // Set the UI Communication as a connection observer
	mNetwork->attachObserver(mMultiplayerManager);				  // Let the MultiplayerManager know of network changes

	mMultiplayerManager->setInternalObservers();

	std::string localIPv4 = mNetwork->getCurrentIPv4();
	mMultiplayerManager->init(localIPv4);
}


void GameManager::stoppedMultiplayer()
{
	mMultiplayerManager->reset();
}


bool GameManager::isMultiplayerActive() const
{
	return mIsMultiplayerMode;
}


PlayerColor GameManager::getCurrentPlayer() const
{
	return mEngine->getCurrentPlayer();
}


bool GameManager::isLocalPlayerTurn()
{
	// If we're in single player mode, it is always our turn
	if (!isMultiplayerActive())
		return true;

	PlayerColor currentPlayer	   = mEngine->getCurrentPlayer();

	bool		isWhitePlayerTurn  = currentPlayer == PlayerColor::White;
	bool		isBlackPlayerTurn  = currentPlayer == PlayerColor::Black;
	bool		isWhiteLocal	   = mEngine->getLocalPlayer() == PlayerColor::White;
	bool		isBlackLocal	   = mEngine->getLocalPlayer() == PlayerColor::Black;

	bool		isLocalPlayersTurn = (isWhitePlayerTurn && isWhiteLocal || isBlackPlayerTurn && isBlackLocal);

	LOG_INFO("Local Player's turn: {}", LoggingHelper::boolToString(isLocalPlayersTurn).c_str());
	LOG_DEBUG("White Player's turn: {}", LoggingHelper::boolToString(isWhitePlayerTurn).c_str());
	LOG_DEBUG("Black Player's turn: {}", LoggingHelper::boolToString(isBlackPlayerTurn).c_str());

	return isLocalPlayersTurn;
}


void GameManager::startRemoteDiscovery(bool isHost)
{
	if (isHost)
	{
		LOG_INFO("Starting to host a session..");
		mMultiplayerManager->hostSession();
	}
	else
	{
		LOG_INFO("Starting to join a session..");
		mMultiplayerManager->startClient();
	}
}


void GameManager::answerConnectionInvitation(bool accepted)
{
	if (!mMultiplayerManager)
		return;

	std::string reason = "";

	if (!accepted)
		reason = "User declined invitation!";

	mMultiplayerManager->sendConnectResponse(accepted, reason);
}


void GameManager::sendConnectionRequestToHost()
{
	if (!mMultiplayerManager)
		return;

	LOG_DEBUG("Sending connection request to the host..");
	mMultiplayerManager->joinSession();
}


void GameManager::setLocalPlayerInMultiplayer(PlayerColor localPlayer)
{
	mMultiplayerManager->localPlayerChosen(localPlayer);
}


void GameManager::setLocalPlayerReady(const bool flag)
{
	mMultiplayerManager->localReadyFlagSet(flag);
}


void GameManager::initObservers()
{
	mEngine->attachObserver(mUiCommunicationLayer);

	mEngine->mWhitePlayer.attachObserver(mUiCommunicationLayer);
	mEngine->mBlackPlayer.attachObserver(mUiCommunicationLayer);
	mEngine->mMoveExecution->attachObserver(mUiCommunicationLayer);

	StateMachine::GetInstance()->attachObserver(mUiCommunicationLayer);

	mEngine->mCPUPlayer->attachObserver(StateMachine::GetInstance());
}


void GameManager::initMultiplayerObservers()
{
	if (!mMultiplayerManager)
	{
		LOG_WARNING("Could not set up the observers, since the Multiplayer Manager is not set up yet!");
		return;
	}

	mEngine->mMoveExecution->attachObserver(mMultiplayerManager->mRemoteSender);	   // Moves will be sent to the remote
	mMultiplayerManager->mRemoteReceiver->attachObserver(StateMachine::GetInstance()); // Received moves will be handled in the state machine

	LOG_DEBUG("Mulitplayer observers set up!");
}


void GameManager::deinitObservers()
{
	mEngine->detachObserver(mUiCommunicationLayer);

	mEngine->mWhitePlayer.detachObserver(mUiCommunicationLayer);
	mEngine->mBlackPlayer.detachObserver(mUiCommunicationLayer);
	mEngine->mMoveExecution->detachObserver(mUiCommunicationLayer);

	StateMachine::GetInstance()->detachObserver(mUiCommunicationLayer);
}


void GameManager::setSFXEnabled(const bool enabled)
{
	mUserSettings.setSFXEnabled(enabled);
}


bool GameManager::getSFXEnabled()
{
	return mUserSettings.getSFXEnabled();
}


void GameManager::setAtmosEnabled(const bool enabled)
{
	mUserSettings.setAtmosEnabled(enabled);
}


bool GameManager::getAtmosEnabled()
{
	return mUserSettings.getAtmosEnabled();
}


void GameManager::setSFXVolume(const float volume)
{
	mUserSettings.setSFXVolume(volume);
}


float GameManager::getSFXVolume()
{
	return mUserSettings.getSFXVolume();
}


void GameManager::setAtmosVolume(const float volume)
{
	mUserSettings.setAtmosVolume(volume);
}


float GameManager::getAtmosVolume()
{
	return mUserSettings.getAtmosVolume();
}


void GameManager::setMasterAudioVolume(const float volume)
{
	mUserSettings.setMasterAudioVolume(volume);
}


float GameManager::getMasterVolume()
{
	return mUserSettings.getMasterVolume();
}


void GameManager::setAtmosScenario(const std::string scenario)
{
	mUserSettings.setAtmosScenario(scenario);
}


std::string GameManager::getAtmosScenario()
{
	return mUserSettings.getAtmosScenario();
}


void GameManager::setGameConfiguration(GameConfiguration config)
{
	mConfig = config;
}


bool GameManager::startCPUGame()
{
	LOG_INFO("Game started against CPU - Difficulty: {}", mConfig.difficulty);

	PlayerColor		 cpuColor = mConfig.localPlayer == PlayerColor::White ? PlayerColor::Black : PlayerColor::White;

	CPUConfiguration CPUConfig;
	CPUConfig.difficulty   = static_cast<CPUDifficulty>(mConfig.difficulty);
	CPUConfig.enabled	   = true;
	CPUConfig.cpuColor	   = cpuColor;
	CPUConfig.thinkingTime = std::chrono::milliseconds(1000);

	mEngine->setCPUConfiguration(CPUConfig);

	mEngine->startGame();

	return true;
}


bool GameManager::isCPUPlayer(PlayerColor player) const
{
	return mEngine->isCPUPlayer(player);
}


void GameManager::requestCPUMoveAsync(PlayerColor player)
{
	return mEngine->requestCPUMoveAsync(player);
}
