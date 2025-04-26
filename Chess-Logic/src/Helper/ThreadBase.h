/*
  ==============================================================================
	Module:         ThreadBase
	Description:    Base class for thread management with event triggering
  ==============================================================================
*/

#pragma once

#include <boost/thread.hpp>
#include <mutex>
#include <atomic>
#include <condition_variable>


class ThreadBase
{
public:
	ThreadBase()		  = default;
	virtual ~ThreadBase() = default;

	void start()
	{
		if (isRunning())
			return;

		mRunning.store(true);
		mThread = boost::thread(&ThreadBase::run, this);
	}

	void stop()
	{
		if (!isRunning())
			return;

		mRunning.store(false);
		triggerEvent(); // Wake up the thread

		if (mThread.joinable())
			mThread.join();
	}

	void triggerEvent()
	{
		{
			std::lock_guard<std::mutex> lock(mMutex);
			mEventTriggered = true;
		}
		cv.notify_one();
	}

protected:
	// Needs to be implemented by the derived class!!
	virtual void run() = 0;

	bool		 waitForEvent(unsigned long timeoutMS = 0)
	{
		std::unique_lock<std::mutex> lock(mMutex);

		if (timeoutMS > 0)
		{
			cv.wait_for(lock, std::chrono::milliseconds(timeoutMS), [this] { return mEventTriggered || !isRunning(); });
		}
		else
		{
			cv.wait(lock, [this] { return mEventTriggered || !isRunning(); });
		}
		bool wasTriggered = mEventTriggered;
		mEventTriggered	  = false;			// Reset the flag

		return wasTriggered && isRunning(); // Return true if event was triggered and thread is still running
	}

	bool isRunning() const { return mRunning.load(); }

private:
	boost::thread			mThread;
	std::atomic<bool>		mRunning;
	std::mutex				mMutex;
	std::condition_variable cv;
	bool					mEventTriggered{false};
};