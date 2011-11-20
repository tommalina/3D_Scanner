/*
 * ThreadManager.cpp
 *
 *  Created on: 10-09-2011
 *      Author: ble
 */

#include "ThreadManager.h"
#include <stdlib.h>

ThreadManager::ThreadList::ThreadList()
{
	mObjRun			= NULL;
	mNext			= NULL;
	mThread			= NULL;
}

ThreadManager::ThreadList::~ThreadList()
{
	mObjRun			= NULL;
	mNext			= NULL;
	mThread			= NULL;
}

ThreadManager::ThreadManager()
{
	mThreadList		= NULL;
}

ThreadManager::~ThreadManager()
{
	ThreadList** i				= &mThreadList;
	while(*i)
	{
		if((*i)->mThread!=NULL)
		{
			(*i)->mThread->join();
			delete (*i)->mThread;
		}
		ThreadList* next		= (*i)->mNext;
		delete (*i);
		mThreadList			= next;
		i					= &mThreadList;
	}
}

std::auto_ptr<ThreadManager>	ThreadManager::mInstance;

ThreadManager* ThreadManager::getInstance()
{
	if(mInstance.get()==NULL)
	{
		mInstance.reset(new ThreadManager());
	}
	return mInstance.get();
}

void ThreadManager::addThread(ThreadRunnable* objRun)
{
	ThreadList** i									= &(ThreadManager::getInstance()->mThreadList);
	for(;*i;i=&((*i)->mNext));
	(*i)											= new ThreadList();
	(*i)->mObjRun									= objRun;

}

void ThreadManager::startThread(ThreadRunnable* objRun)
{
	for(ThreadList* i = ThreadManager::getInstance()->mThreadList; i != NULL ; i = i->mNext)
	{
		if(i->mObjRun==objRun) {
			if(i->mThread==NULL) {
				ThreadBoost threadBoost(i->mObjRun);
				i->mThread		= new boost::thread(threadBoost);
			}
			break;
		}
	}
}

void ThreadManager::startAll()
{
	for(ThreadList* i = ThreadManager::getInstance()->mThreadList; i != NULL ; i = i->mNext)
	{
		if(i->mThread==NULL) {
			ThreadBoost threadBoost(i->mObjRun);
			i->mThread		= new boost::thread(threadBoost);
		}
	}
}

void ThreadManager::endThread(ThreadRunnable* objRun)
{
	for(ThreadList** i = &(ThreadManager::getInstance()->mThreadList); *i ; i = &((*i)->mNext))
	{
		if((*i)->mObjRun==objRun) {
			if((*i)->mThread!=NULL) {
				(*i)->mThread->join();
				delete (*i)->mThread;
			}
			ThreadList *next 		= (*i)->mNext;
			delete (*i);
			(*i) = next;
			break;
		}
	}
}

void ThreadManager::endNowThread(ThreadRunnable* objRun)
{
	for(ThreadList** i = &(ThreadManager::getInstance()->mThreadList); *i ; i = &((*i)->mNext))
	{
		if((*i)->mObjRun==objRun) {
			(*i)->mObjRun->end();
			if((*i)->mThread!=NULL) {
				(*i)->mThread->join();
				delete (*i)->mThread;
			}
			ThreadList *next 		= (*i)->mNext;
			delete (*i);
			(*i) = next;
			break;
		}
	}
}

void ThreadManager::endAll()
{
	while (ThreadManager::getInstance()->mThreadList) {
		ThreadList* i				= ThreadManager::getInstance()->mThreadList;
		ThreadList* next			= i->mNext;
		if(i->mThread!=NULL) {
			i->mThread->join();
			delete i->mThread;
		}
		delete i;
		ThreadManager::getInstance()->mThreadList		= next;
	}
}

void ThreadManager::endNowAll()
{
	while (ThreadManager::getInstance()->mThreadList) {
		ThreadList* i				= ThreadManager::getInstance()->mThreadList;
		ThreadList* next			= i->mNext;
		i->mObjRun->end();
		if(i->mThread!=NULL) {
			i->mThread->join();
			delete i->mThread;
		}
		delete i;
		ThreadManager::getInstance()->mThreadList		= next;
	}
}
