/*
  ==============================================================================
	Module:         GameManager
	Description:    Manager for the Chess game
  ==============================================================================
*/

#pragma once

#include "GameEngine.h"
#include "Player.h"
#include "Logging.h"
#include "UserSettings.h"
#include "IObservable.h"
#include "UICommunication.h"
#include "NetworkManager.h"
#include "MultiplayerManager.h"
#include "PlayerName.h"
#include "SystemInfo.h"


class StateMachine;

/**
 * @brief	Orchestrates overall game lifecycle, player turns, move execution, configuration,
 *			multiplayer bridging, audio/user settings, and CPU assistance. Singleton.
 */
class GameManager
{
public:
	~GameManager();

	static GameManager		   *GetInstance();
	static void					ReleaseInstance();

	/**
	 * @brief	Initialize subsystems (engine, settings, networking as needed).
	 * @return	true if initialization completes successfully.
	 */
	bool						init();

	/**
	 * @brief	Start a new game based on current configuration.
	 * @return	true if game successfully started.
	 */
	bool						startGame();

	/**
	 * @brief	Execute a (validated) move. Applies to board, updates history, triggers observers.
	 * @param	tmpMove -> Move to execute (may be updated with promotion info).
	 * @param	fromRemote -> True if originating from remote multiplayer peer.
	 */
	void						executeMove(PossibleMove &tmpMove, bool fromRemote = false);

	/**
	 * @brief	Undo the last reversible move (if permitted).
	 */
	void						undoMove();

	/**
	 * @brief	Reset current game state (board, history, flags) while keeping configuration.
	 */
	void						resetGame();

	/**
	 * @brief	Return winner color if game ended.
	 * @return	Winner player color, or nullopt if still ongoing
	 */
	std::optional<PlayerColor>	getWinner() const;

	/**
	 * @brief	Register a native callback delegate (interop / UI integration).
	 */
	void						setDelegate(PFN_CALLBACK pDelegate);

	/**
	 * @brief	Generate all legal moves for currently selected square (if any).
	 * @return	Vector of possible moves; empty if none.
	 */
	std::vector<PossibleMove>	getPossibleMoveForPosition();

	/**
	 * @brief	Copy current board state into provided array.
	 * @param	boardState -> 2D array [BOARD_SIZE][BOARD_SIZE].
	 * @return	true if state written.
	 */
	bool						getBoardState(int boardState[BOARD_SIZE][BOARD_SIZE]);

	/**
	 * @brief	Validate move semantics without executing.
	 */
	bool						checkForValidMoves(const PossibleMove &move);

	/**
	 * @brief	Check if a move is a pawn promotion candidate.
	 */
	bool						checkForPawnPromotionMove(const PossibleMove &move);

	/**
	 * @brief	Enumerate detected network adapters.
	 */
	std::vector<NetworkAdapter> getNetworkAdapters();

	/**
	 * @brief	Switch active network adapter by ID.
	 * @return	true on success.
	 */
	bool						changeCurrentNetworkAdapter(int ID);

	/**
	 * @brief	Current network adapter ID (or -1 if none).
	 */
	int							getCurrentNetworkAdapterID();

	/**
	 * @brief	Set local player name (stored & propagated to multiplayer if active).
	 */
	void						setLocalPlayerName(std::string name);

	/**
	 * @brief	Get local player name.
	 */
	std::string					getLocalPlayerName();

	void						setBoardTheme(std::string theme) { mUserSettings.setCurrentBoardTheme(theme); }
	std::string					getBoardTheme() { return mUserSettings.getCurrentBoardTheme(); }

	void						setPieceTheme(std::string theme) { mUserSettings.setCurrentPieceTheme(theme); }
	std::string					getPieceTheme() { return mUserSettings.getCurrentPieceTheme(); }

	/**
	 * @brief	Advance turn to next player (handles multiplayer / CPU specifics).
	 */
	void						switchTurns();

	/**
	 * @brief	Pre-calculate all legal moves for current player (caching).
	 * @return	true if successful.
	 */
	bool						calculateAllMovesForPlayer();

	/**
	 * @brief	Begin user move by selecting a starting square.
	 * @return	true if selection is valid and move initiation accepted.
	 */
	bool						initiateMove(const Position &startPosition);

	/**
	 * @brief	Evaluate current board for end-game conditions.
	 * @return	Returns the EndGameState. If game is ongoing, it iwll return EndGameState::Ongoing.
	 */
	EndGameState				checkForEndGameConditions();

	/**
	 * @brief	Start multiplayer chess game.
	 * @return	true if session start initiated.
	 */
	bool						startMultiplayerGame();

	/**
	 * @brief	Disconnect active multiplayer session (if any).
	 */
	void						disconnectMultiplayerGame();

	/**
	 * @brief	Starts the flow of the Multiplayer game. The user selected a MP game.
	 */
	void						startedMultiplayer();

	/**
	 * @brief	Stops the flow of the Multiplayer game.
	 */
	void						stoppedMultiplayer();

	/**
	 * @brief	Whether multiplayer mode currently active.
	 */
	bool						isMultiplayerActive() const;

	/**
	 * @brief	Player whose turn it is.
	 */
	PlayerColor					getCurrentPlayer() const;

	/**
	 * @brief	True if it's the local player's turn (handles color mapping in multiplayer).
	 */
	bool						isLocalPlayerTurn();

	/**
	 * @brief	Initiate discovery of remote sessions (host or client perspective).
	 * @param	isHost -> True if acting as session host.
	 */
	void						startRemoteDiscovery(bool isHost);

	/**
	 * @brief	Respond to an incoming connection invitation.
	 * @param	accepted -> True to accept, false to reject.
	 */
	void						answerConnectionInvitation(bool accepted);

	/**
	 * @brief	Issue connection request to the detected host.
	 */
	void						sendConnectionRequestToHost();

	/**
	 * @brief	Assign local player's color in multiplayer context.
	 */
	void						setLocalPlayerInMultiplayer(PlayerColor localPlayer);

	/**
	 * @brief	Set ready flag for local player (multiplayer game start gating).
	 */
	void						setLocalPlayerReady(const bool flag);

	/**
	 * @brief	Start game vs CPU opponent.
	 * @return	true if CPU mode started.
	 */
	bool						startCPUGame();

	/**
	 * @brief	Determine if specified player is CPU-controlled.
	 */
	bool						isCPUPlayer(PlayerColor player) const;

	/**
	 * @brief	Asynchronously request a CPU move (non-blocking).
	 */
	void						requestCPUMoveAsync();


	void						setSFXEnabled(const bool enabled);
	bool						getSFXEnabled();

	void						setAtmosEnabled(const bool enabled);
	bool						getAtmosEnabled();

	void						setSFXVolume(const float volume);
	float						getSFXVolume();

	void						setAtmosVolume(const float volume);
	float						getAtmosVolume();

	void						setMasterAudioVolume(const float volume);
	float						getMasterVolume();

	void						setAtmosScenario(const std::string scenario);
	std::string					getAtmosScenario();

	/**
	 * @brief	Apply new game configuration (affects next start / restart).
	 */
	void						setGameConfiguration(GameConfiguration config);

private:
	GameManager() = default;

	void								initObservers();
	void								initMultiplayerObservers();
	void								deinitObservers();


	Logging								mLog;

	UserSettings						mUserSettings;

	PlayerName							mPlayerName;

	std::shared_ptr<GameEngine>			mEngine;

	std::shared_ptr<UICommunication>	mUiCommunicationLayer;

	std::shared_ptr<MultiplayerManager> mMultiplayerManager;
	std::unique_ptr<NetworkManager>		mNetwork;

	GameConfiguration					mConfig;

	bool								mIsMultiplayerMode{false};
};
