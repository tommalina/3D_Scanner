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

using namespace std;
using namespace boost;

/**
 * Pure virtual class, that is interface for object in new thread.
 */
class ThreadRunnable {
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
};

/**
 * Singleton class that is used to manage of all threads in application.
 */
class ThreadManager {
private:

	/**
	 * Smart Pointer to only one instance of ThreadManager.
	 */
	static auto_ptr<ThreadManager>				mInstance;

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
		thread*				mThread;

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
	 * @param data - Pointer to data that will be send to run method in ThreadRunable object.
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

#endif /* THREADMANAGER_H_ */
