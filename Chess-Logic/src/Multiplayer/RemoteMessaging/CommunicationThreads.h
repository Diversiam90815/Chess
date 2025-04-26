/*
  ==============================================================================
	Module:         CommunicationThreads
	Description:    Threads used for remote communication (sending and receiving data)
  ==============================================================================
*/

#pragma once

#include "ThreadBase.h"

class RemoteCommunication;


class SendThread : public ThreadBase
{
public:
	SendThread(RemoteCommunication *owner);

protected:
	void run() override;

private:
	RemoteCommunication *mOwner = nullptr;
};


class ReceiveThread : public ThreadBase
{
public:
	ReceiveThread(RemoteCommunication *owner);

protected:
	void run() override;

private:
	RemoteCommunication *mOwner = nullptr;
};
