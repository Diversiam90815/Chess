/*
  ==============================================================================
	Module:         CommunicationThreads
	Description:    Threads used for remote communication (sending and receiving data)
  ==============================================================================
*/

#pragma once

#include "ThreadBase.h"

class RemoteCommunication;

/// <summary>
/// A thread class responsible for handling sending operations in remote communication.
/// </summary>
class SendThread : public ThreadBase
{
public:
	SendThread(RemoteCommunication *owner);

protected:
	void run() override;

private:
	RemoteCommunication *mOwner = nullptr;
};

/// <summary>
/// A thread class responsible for handling data reception in remote communication.
/// </summary>
class ReceiveThread : public ThreadBase
{
public:
	ReceiveThread(RemoteCommunication *owner);

protected:
	void run() override;

private:
	RemoteCommunication *mOwner = nullptr;
};
