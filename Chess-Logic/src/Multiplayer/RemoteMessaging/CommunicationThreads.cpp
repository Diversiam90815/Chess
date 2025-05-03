/*
  ==============================================================================
	Module:         CommunicationThreads
	Description:    Threads used for remote communication (sending and receiving data)
  ==============================================================================
*/

#include "CommunicationThreads.h"
#include "RemoteCommunication.h"


SendThread::SendThread(RemoteCommunication *owner) : mOwner(owner) {}


void SendThread::run()
{
	while (isRunning())
	{
		if (!mOwner)
			return;

		mOwner->sendMessages();
		waitForEvent(200);
	}
}


ReceiveThread::ReceiveThread(RemoteCommunication *owner) : mOwner(owner) {}


void ReceiveThread::run()
{
	while (isRunning())
	{
		if (!mOwner)
			return;

		mOwner->receiveMessages();
		waitForEvent(50);
	}
}
