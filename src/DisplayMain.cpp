/*
 * DisplayMain.cpp
 *
 *  Created on: 28-11-2011
 *      Author: ble
 */

#include "DisplayMain.h"
#include <stdio.h>

DisplayMain::DisplayMain()
{
	mRun		= true;
}

DisplayMain::~DisplayMain()
{

}

void DisplayMain::run()
{
	while(mRun)
	{
		printf(" --------- DisplayMain \n");
	}
}

void DisplayMain::end()
{
	mRun		= false;

	printf(" --------- END !!! DisplayMain \n");
}

