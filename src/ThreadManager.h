/*
 * ThreadManager.h
 *
 *  Created on: 10-09-2011
 *      Author: ble (Tomasz Malinowski)
 */

#ifndef THREADMANAGER_H_
#define THREADMANAGER_H_

#include <memory>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

/**
 * Pure virtual class, that is interface for object in new thread.
 */
class ThreadRunnable
{
public:

	/**
	 * Method that will be called in new thread.
	 */
	virtual void run() = 0;

	/**
	 * Method called when thread want to end his life.
	 */
	virtual void end() = 0;

	/**
	 * Virtual destructor.
	 */
	virtual ~ThreadRunnable() {};

	/**
	 *
	 * @param Integer. Amount of milliseconds.
	 */
	void sleep(int);
};

/**
 * Singleton class that is used to manage of all threads in application.
 */
class ThreadManager
{
private:

	/**
	 * Smart Pointer to only one instance of ThreadManager.
	 */
	static std::auto_ptr<ThreadManager>			mInstance;

	/**
	 * Method used to get pointer to ThreadManager.
	 */
	static ThreadManager* 						getInstance();

	/**
	 * Private class that supports all list functionality.
	 */
	class ThreadList {
	public:

		/**
		 * Pointer to next element in list.
		 */
		ThreadList*			mNext;

		/**
		 * Pointer to object that will be called in new thread.
		 */
		ThreadRunnable*		mObjRun;

		/**
		 * Pointer to boost thread.
		 */
		boost::thread*		mThread;

		/**
		 * Public constructor.
		 */
		ThreadList();

		/**
		 * Public destructor.
		 */
		~ThreadList();


	};

	/**
	 * Class that will be argument for boost::thread constructor.
	 */
	class ThreadBoost {
	private:

		/**
		 * Private pointer to ThreadRunable object.
		 */
		ThreadRunnable*		mThreadRunnable;

		/**
		 * Private constructor.
		 */
		ThreadBoost() {};

	public:

		/**
		 * Constructor of ThreadBoost class.
		 * @param threadRunnable - Pointer to ThreadRnnable object that will be run in boost thread.
		 */
		ThreadBoost(ThreadRunnable* threadRunnable)
		{
			mThreadRunnable			= threadRunnable;
		}

		/**
		 * Function operator, that will be called in boost thread.
		 */
		void operator()()
		{
			mThreadRunnable->run();
		}

	};

	/**
	 * Pointer to list with threads data and information.
	 */
	ThreadList*				mThreadList;

	/**
	 * Private constructor.
	 */
	ThreadManager();

	/**
	 * Private constructor.
	 */
	ThreadManager(ThreadManager&) {};

public:

	/**
	 * Public destructor.
	 */
	~ThreadManager();

	/**
	 * Method used to add new thread.
	 * @param objRun - Pointer to ThreadRunable object, that will be called in new thread.
	 */
	static void 								addThread(ThreadRunnable* objRun);

	/**
	 * Method used to start specific thread.
	 * @param objRun - Pointer to ThreadRunable object, used to recognize thread that we want to start.
	 */
	static void									startThread(ThreadRunnable* objRun);

	/**
	 * Method used to end specific thread.
	 * @param objRun - Object of ThreadRunable that is used to recognize thread that we want to stop.
	 */
	static void 								endThread(ThreadRunnable* objRun);

	/**
	 * Method used to end now specific thread.
	 * @param objRun - Object of ThreadRunable that is used to recognize thread that we want to stop.
	 */
	static void 								endNowThread(ThreadRunnable* objRun);

	/**
	 * Method used to start all added threads.
	 */
	static void 								startAll();

	/**
	 * Method used to end all added threads.
	 */
	static void									endAll();

	/**
	 * Method used to end all now added threads.
	 */
	static void									endNowAll();


};

/**
 * Class which supports boost mutex functionality. Behavior of this class is similar to simple Smart Pointer class.
 * This class do not delete object given in constructor. Also this is a template class.
 */

template <typename T>
class ThreadDataUnit
{
	/**
	 * Template pointer to data object.
	 */
	T*				mPtr;

	/**
	 * Mutex object, used to synchronize access to data pointer.
	 */
	boost::mutex	mMutex;

public:

	/**
	 * Constructor.
	 */
	ThreadDataUnit()
	{
		lockData();
		mPtr		= NULL;
		unlockData();
	}

	/**
	 * Constructor.
	 * @param ptr - Pointer to data object which will be managed by ThreadDataUnit object.
	 */
	ThreadDataUnit(T* ptr)
	{
		lockData();
		mPtr		= ptr;
		unlockData();
	}

	/**
	 * Method used to set new pointer in ThreadDataUnit object. When we set new pointer, object under old pointer is not released.
	 * @param ptr - New pointer.
	 */
	void setPtr(T* ptr)
	{
		lockData();
		mPtr		= ptr;
		unlockData();
	}

	/**
	 * Method used to get actual pointer.
	 */
	T* getPtr()
	{
		return mPtr;
	}

	/**
	 * Method used to delete and set to NULL actual object under ptr pointer.
	 */
	void removePtr()
	{
		lockData();
		if(mPtr!=NULL)
		{
			delete mPtr;
			mPtr		= NULL;
		}
		unlockData();
	}

	T* operator->()
	{
		return mPtr;
	}

	T& operator*()
	{
		return *mPtr;
	}

	/**
	 * Method used to lock access to data.
	 */
	void lockData()
	{
		mMutex.lock();
	}

	/*
	 * Method used to try lock access to data.
	 */
	void tryLockData()
	{
		mMutex.try_lock();
	}

	/**
	 * Method used to unlock data.
	 */
	void unlockData()
	{
		mMutex.unlock();
	}
};

#endif /* THREADMANAGER_H_ */
