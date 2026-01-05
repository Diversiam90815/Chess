/*
  ==============================================================================
	Module:         GameManager
	Description:    Manager for the Chess game
  ==============================================================================
*/

#include "GameManager.h"


GameManager::~GameManager()
{
	shutDown();
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
	if (mInitialized)
		return true;

	mLog.initLogging();

	SystemInfo::logSystemInfo();

	mUserSettings.init();

	initializeComponents();
	wireComponents();

	mInitialized = true;
	LOG_INFO("GameManager initialized!");

	return true;
}


void GameManager::shutDown()
{
	if (!mInitialized)
		return;

	LOG_INFO("Shutting down Game Manager!");

	if (mStateMachine)
		mStateMachine->stop();

	if (mGameController)
		mGameController->cancelCPUCalculation();

	mInitialized = false;
}


void GameManager::startGame(GameConfiguration config)
{
	LOG_INFO("Starting game. Mode : {}", static_cast<int>(config.mode)); // TODO: Add string conversion to LoggingHelper
	mStateMachine->onGameStart(config);
}


void GameManager::setDelegate(PFN_CALLBACK pDelegate)
{
	mInputSource->setDelegate(pDelegate);
}


std::array<PieceType, 64> GameManager::getBoardPieces() const
{
	std::array<PieceType, 64> pieces{};

	const Chessboard		 &board = mGameController->getBoard();

	for (int i = 0; i < 64; ++i)
	{
		pieces[i] = board.pieceAt(static_cast<Square>(i));
	}

	return pieces;
}


const MoveList &GameManager::getCachedLegalMoves() const
{
	return mGameController->getCachedLegalMoves();
}


PieceType GameManager::getPieceAt(Square sq) const
{
	return mGameController->getBoard().pieceAt(sq);
}


void GameManager::undoMove()
{
	mStateMachine->onUndoRequested();
}


void GameManager::onSquareSelected(Square sq)
{
	mStateMachine->onSquareSelected(sq);
}


void GameManager::onPromotionChosen(PieceType piece)
{
	mStateMachine->onPromotionChosen(piece);
}



void GameManager::resetGame()
{
	mStateMachine->onGameReset();
}


std::vector<NetworkAdapter> GameManager::getNetworkAdapters()
{
	return mNetworkManager->getAvailableNetworkAdapters();
}


bool GameManager::changeCurrentNetworkAdapter(int ID)
{
	return mNetworkManager->changeCurrentNetworkAdapter(ID);
}


int GameManager::getCurrentNetworkAdapterID()
{
	return mNetworkManager->getCurrentNetworkAdapterID();
}


void GameManager::startMultiplayerSession()
{
	// The player clicked on the Multiplayer menu button and started the multiplayer

	LOG_INFO("Multiplayer started..");

	mMultiplayerManager = std::make_shared<MultiplayerManager>(); // Create the multiplayer manager
	setupMultiplayObservers();

	std::string localIPv4 = mNetworkManager->getCurrentIPv4();
	mMultiplayerManager->init(localIPv4);
}


void GameManager::stoppedMultiplayer()
{
	if (mMultiplayerManager)
	{
		mMultiplayerManager->disconnect();
		mMultiplayerManager.reset();
	}


	mIsMultiplayerMode = false;
	resetGame(); // Reset to single player}
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


void GameManager::setLocalPlayerInMultiplayer(Side localPlayer)
{
	mMultiplayerManager->localPlayerChosen(localPlayer);
}


void GameManager::setLocalPlayerReady(const bool flag)
{
	mMultiplayerManager->localReadyFlagSet(flag);
}


void GameManager::initializeComponents()
{
	mInputSource	= std::make_shared<WinUIInputSource>();
	mGameController = std::make_unique<GameController>();
	mStateMachine	= std::make_unique<StateMachine>();
	mNetworkManager = std::make_unique<NetworkManager>();

	mNetworkManager->init();
}


void GameManager::wireComponents()
{
	// Wire StateMachine to use GameController and InputSource
	mStateMachine->setGameController(mGameController.get());
	mStateMachine->setInputSource(mInputSource.get());

	// CPU moves flow back through StateMachine
	mGameController->setCPUMoveCallback([this](Move move) { mStateMachine->onCPUMoveCalculated(move); });

	mStateMachine->start();
}


void GameManager::setupMultiplayObservers()
{
	if (!mMultiplayerManager)
	{
		LOG_WARNING("Could not set up the observers, since the Multiplayer Manager is not set up yet!");
		return;
	}

	// mEngine->mMoveExecution.attachObserver(mMultiplayerManager->mRemoteSender);		   // TODO: Moves will be sent to the remote

	// Remote moves -> StateMachine
	mMultiplayerManager->setRemoteMoveCallback([this](Move move) { mStateMachine->onRemoteMoveReceived(move); });

	mMultiplayerManager->attachObserver(mInputSource);	  // Connection status -> UI
	mNetworkManager->attachObserver(mMultiplayerManager); // Let the MultiplayerManager know of network changes

	mMultiplayerManager->setInternalObservers();

	LOG_DEBUG("Mulitplayer observers set up!");
}
