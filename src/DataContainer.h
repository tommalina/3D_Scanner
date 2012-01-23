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

	virtual ThreadDataUnit<IplImage>&	getImageLeftRef() = 0;
	virtual ThreadDataUnit<IplImage>&	getImageRightRef() = 0;
	virtual ThreadDataUnit<IplImage>&	getImageLeftGrayRef() = 0;
	virtual ThreadDataUnit<IplImage>&	getImageRightGrayRef() = 0;
	virtual ThreadDataUnit<bool>&		drawChessboard() = 0;
	virtual	ThreadDataUnit<short>&		getCalibrateDataAccess() = 0;
	virtual ThreadDataUnit<CvPoint2D32f>&		getCalibrateCurrentPointsLeft() = 0;
	virtual ThreadDataUnit<CvPoint2D32f>&		getCalibrateCurrentPointsRight() = 0;
	virtual ThreadDataUnit<int>&		getCalibrateChessboardW() = 0;
	virtual ThreadDataUnit<int>&		getCalibrateChessboardH() = 0;
	virtual ThreadDataUnit<int>&		getCalibrateCurrentCornersLeft() = 0;
	virtual ThreadDataUnit<int>&		getCalibrateCurrentCornersRight() = 0;
	virtual ThreadDataUnit<int>&		getCalibrateResultLeft() = 0;
	virtual ThreadDataUnit<int>&		getCalibrateResultRight() = 0;
	virtual ThreadDataUnit<CvMat>&		getCalibrateX1() = 0;
	virtual ThreadDataUnit<CvMat>&		getCalibrateY1() = 0;
	virtual ThreadDataUnit<CvMat>&		getCalibrateX2() = 0;
	virtual ThreadDataUnit<CvMat>&		getCalibrateY2() = 0;
	virtual ThreadDataUnit<bool>&		getStart3D() = 0;
	virtual ThreadDataUnit<CvMat>&		getDepthMap() = 0;

	virtual ~DataContainer(){};

};
#endif /* DATACONTAINER_H_ */
