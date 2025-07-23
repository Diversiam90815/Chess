/*
  ==============================================================================
	Module:         ChessLogicAPI
	Description:    Plain C API for the Chess logic
  ==============================================================================
*/


#include "EngineAPIDefines.h"


#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

	Engine_API void	 Init();

	Engine_API void	 Deinit();


	Engine_API void	 SetDelegate(PFN_CALLBACK pDelegate);

	Engine_API float GetWindowScalingFactor(HWND hwnd);

	Engine_API void	 SetUnvirtualizedAppDataPath(const char *appDataPath);


	Engine_API int	 GetNumPossibleMoves();

	Engine_API bool	 GetPossibleMoveAtIndex(int index, PossibleMoveInstance *possibleMoveInstance);


	Engine_API void	 OnSquareSelected(PositionInstance positionInstance);

	Engine_API void	 OnPawnPromotionChosen(PieceTypeInstance promotionInstance);


	Engine_API void	 StartGame(GameConfiguration config);

	Engine_API void	 ResetGame();

	Engine_API void	 UndoMove();

	Engine_API int	 GetEndgameState();

	Engine_API bool	 GetBoardState(int *boardstate);


	Engine_API void	 StartedMultiplayer();

	Engine_API void	 StartMultiplayerGame();

	Engine_API void	 StartRemoteDiscovery(bool isHost);

	Engine_API void	 DisconnectMultiplayerGame();

	Engine_API void	 AnswerConnectionInvitation(bool accept);

	Engine_API void	 SendConnectionRequestToHost();

	Engine_API void	 StoppedMultiplayer();

	Engine_API void	 SetLocalPlayer(int iLocalPlayer);

	Engine_API void	 SetLocalPlayerReady(bool ready);


	Engine_API void	 LogInfoWithCaller(const char *message, const char *method, const char *className, const int lineNumber);

	Engine_API void	 LogErrorWithCaller(const char *message, const char *method, const char *className, const int lineNumber);

	Engine_API void	 LogWarningWithCaller(const char *message, const char *method, const char *className, const int lineNumber);


	Engine_API void	 SetCurrentBoardTheme(const char *theme);

	Engine_API char *GetCurrentBoardTheme();

	Engine_API void	 SetCurrentPieceTheme(const char *theme);

	Engine_API char *GetCurrentPieceTheme();

	Engine_API void	 SetLocalPlayerName(const char *name);

	Engine_API char *GetLocalPlayerName();

	Engine_API bool	 GetSFXEnabled();

	Engine_API void	 SetSFXEnabled(bool enabled);

	Engine_API bool	 GetAtmosEnabled();

	Engine_API void	 SetAtmosEnabled(bool enabled);

	Engine_API void	 SetSFXVolume(float volume);

	Engine_API float GetSFXVolume();

	Engine_API void	 SetAtmosVolume(float volume);

	Engine_API float GetAtmosVolume();

	Engine_API void	 SetAtmosScenario(const char *scenario);

	Engine_API char *GetAtmosScenario();

	Engine_API void	 SetMasterVolume(float volume);

	Engine_API float GetMasterVolume();


	Engine_API int	 GetNetworkAdapterCount();

	Engine_API bool	 GetNetworkAdapterAtIndex(unsigned int index, NetworkAdapterInstance *adapter);

	Engine_API int	 GetSavedAdapterID();

	Engine_API bool	 ChangeCurrentAdapter(int ID);

#ifdef __cplusplus
}
#endif // __cplusplus
