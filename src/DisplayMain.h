/*
 * DisplayMain.h
 *
 *  Created on: 28-11-2011
 *      Author: ble
 */

#ifndef DISPLAYMAIN_H_
#define DISPLAYMAIN_H_

#include "ThreadManager.h"

class DisplayMain : public ThreadRunnable
{
private:

	bool		mRun;

public:

	DisplayMain();

	virtual ~DisplayMain();

	void run();

	void end();

};

#endif /* DISPLAYMAIN_H_ */
