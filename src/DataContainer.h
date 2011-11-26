/*
 * DataContainer.h
 *
 *  Created on: 26-11-2011
 *      Author: ble
 */

#ifndef DATACONTAINER_H_
#define DATACONTAINER_H_

#include "ThreadManager.h"
#include "Camera.h"
#include <memory>

/**
 * Class that have all necessary data used in application.
 */
class DataContainer
{
private:

	DataContainer(){};

	DataContainer(DataContainer& arg){};

public:

	ThreadDataUnit<Camera>		mCameraL;
	ThreadDataUnit<Camera>		mCameraR;

};

#endif /* DATACONTAINER_H_ */
