/*
 * DataContainer.h
 *
 *  Created on: 07-01-2012
 *      Author: ble
 */

#ifndef DATACONTAINER_H_
#define DATACONTAINER_H_

#include <opencv.hpp>
#include "ThreadManager.h"

class DataContainer
{

public:

	virtual ThreadDataUnit<IplImage>& getImageLeftRef() = 0;
	virtual ThreadDataUnit<IplImage>& getImageRightRef() = 0;
	virtual ThreadDataUnit<IplImage>& getImageLeftGrayRef() = 0;
	virtual ThreadDataUnit<IplImage>& getImageRightGrayRef() = 0;

	virtual ~DataContainer(){};

};
#endif /* DATACONTAINER_H_ */
