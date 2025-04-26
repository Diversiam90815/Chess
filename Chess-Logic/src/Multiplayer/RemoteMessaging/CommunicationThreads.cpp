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
	waitForEvent(200);
	mOwner->sendMessages();
}


ReceiveThread::ReceiveThread(RemoteCommunication *owner) : mOwner(owner) {}


void ReceiveThread::run()
{
	waitForEvent(200);
	mOwner->receiveMessages();
}
