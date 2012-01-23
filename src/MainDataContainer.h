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

private:

	ThreadDataUnit<IplImage>				mLeftImage;
	ThreadDataUnit<IplImage>				mRightImage;

	ThreadDataUnit<IplImage>				mLeftImageGray;
	ThreadDataUnit<IplImage>				mRightImageGray;

	ThreadDataUnit<CvPoint2D32f>			mCalibrateCurrentPointsLeft;
	ThreadDataUnit<CvPoint2D32f>			mCalibrateCurrentPointsRight;

	ThreadDataUnit<bool>					mDrawChessboard;

	ThreadDataUnit<short>					mCalibrateDataAccess;

	ThreadDataUnit<int>						mCalibrateChessboardW;
	ThreadDataUnit<int>						mCalibrateChessboardH;

	ThreadDataUnit<int>						mCalibrateCurrentCornersLeft;
	ThreadDataUnit<int>						mCalibrateCurrentCornersRight;

	ThreadDataUnit<int>						mCalibrateResultLeft;
	ThreadDataUnit<int>						mCalibrateResultRight;

	ThreadDataUnit<CvMat>					mCalibrateX1;
	ThreadDataUnit<CvMat>					mCalibrateY1;
	ThreadDataUnit<CvMat>					mCalibrateX2;
	ThreadDataUnit<CvMat>					mCalibrateY2;

	ThreadDataUnit<bool>					mStart3D;
	ThreadDataUnit<CvMat>					mDepthMap;

public:

	MainDataContainer();

	ThreadDataUnit<IplImage>&	getImageLeftRef();
	ThreadDataUnit<IplImage>&	getImageRightRef();

	ThreadDataUnit<IplImage>&	getImageLeftGrayRef();
	ThreadDataUnit<IplImage>&	getImageRightGrayRef();

	ThreadDataUnit<bool>&		drawChessboard();

	ThreadDataUnit<short>&		getCalibrateDataAccess();

	ThreadDataUnit<CvPoint2D32f>&		getCalibrateCurrentPointsLeft();
	ThreadDataUnit<CvPoint2D32f>&		getCalibrateCurrentPointsRight();

	ThreadDataUnit<int>&		getCalibrateChessboardW();
	ThreadDataUnit<int>&		getCalibrateChessboardH();

	ThreadDataUnit<int>&		getCalibrateCurrentCornersLeft();
	ThreadDataUnit<int>&		getCalibrateCurrentCornersRight();

	ThreadDataUnit<int>&		getCalibrateResultLeft();
	ThreadDataUnit<int>&		getCalibrateResultRight();

	ThreadDataUnit<CvMat>&		getCalibrateX1();
	ThreadDataUnit<CvMat>&		getCalibrateY1();
	ThreadDataUnit<CvMat>&		getCalibrateX2();
	ThreadDataUnit<CvMat>&		getCalibrateY2();

	ThreadDataUnit<bool>&		getStart3D();
	ThreadDataUnit<CvMat>&		getDepthMap();

	virtual ~MainDataContainer();

};



#endif /* 3DDATACONTAINER_H_ */
