/*
 * 3DDataContainer.h
 *
 *  Created on: 07-01-2012
 *      Author: ble
 */

#ifndef MAINDATACONTAINER_H_
#define MAINDATACONTAINER_H_

#include "DataContainer.h"
#include "ThreadManager.h"

class MainDataContainer : public DataContainer {

public:

	ThreadDataUnit<IplImage>	mLeftImage;
	ThreadDataUnit<IplImage>	mRightImage;

	ThreadDataUnit<IplImage>	mLeftImageGray;
	ThreadDataUnit<IplImage>	mRightImageGray;

	ThreadDataUnit<IplImage>&	getImageLeftRef();
	ThreadDataUnit<IplImage>&	getImageRightRef();

	ThreadDataUnit<IplImage>&	getImageLeftGrayRef();
	ThreadDataUnit<IplImage>&	getImageRightGrayRef();

	virtual ~MainDataContainer();

};



#endif /* 3DDATACONTAINER_H_ */
