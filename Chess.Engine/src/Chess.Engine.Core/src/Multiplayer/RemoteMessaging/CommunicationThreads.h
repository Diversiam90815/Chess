/*
  ==============================================================================
	Module:         CommunicationThreads
	Description:    Dedicated worker threads supporting RemoteCommunication.
					Decouple blocking I/O and queue processing from higher level
					game state logic. Each thread cooperatively waits for work
					signaled by RemoteCommunication (event driven).
  ==============================================================================
*/

#pragma once

#include "ThreadBase.h"

class RemoteCommunication;


/**
 * @brief	Processes the outgoing message queue owned by RemoteCommunication.
 *
 * Responsibilities:
 *  - Wait for an event (triggered when new outgoing messages are enqueued).
 *  - Drain / serialize messages through the active ITCPSession.
 *
 * Concurrency:
 *  - Outgoing queue protected by RemoteCommunication's mutex; SendThread only
 *    accesses it inside that synchronized context.
 */
class SendThread : public ThreadBase
{
public:
	explicit SendThread(RemoteCommunication *owner);

protected:
	/**
	 * @brief	Cooperative loop: waits for trigger, flushes current outgoing messages.
	 *			Exits when stop() is requested.
	 */
	void run() override;

private:
	RemoteCommunication *mOwner = nullptr;
};


/**
 * @brief	Manages reception of incoming messages (polling or waiting depending
 *			on session strategy) and forwards them to RemoteCommunication for dispatch.
 *
 * Responsibilities:
 *  - Continuously attempt to read available messages while running.
 *  - Signal observers (via RemoteCommunication) after successful parse.
 *
 * Backpressure:
 *  - Incoming messages appended to a bounded / implicit queue (vector). If
 *    scaling becomes an issue consider lock-free or ring buffer structure.
 */
class ReceiveThread : public ThreadBase
{
public:
	explicit ReceiveThread(RemoteCommunication *owner);

protected:
	/**
	 * @brief	Loop attempts to receive and enqueue messages; waits (sleep or event)
	 *			when no data and remains responsive to stop().
	 */
	void run() override;

private:
	RemoteCommunication *mOwner = nullptr;
};
