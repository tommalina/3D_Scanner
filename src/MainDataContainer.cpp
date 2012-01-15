
#include "MainDataContainer.h"

MainDataContainer::MainDataContainer()
{
	mDrawChessboard.setPtr(new bool(false));
	mCalibrateChessboardW.setPtr(new int(0));
	mCalibrateChessboardH.setPtr(new int(0));
	mCalibrateCurrentCornersLeft.setPtr(new int(0));
	mCalibrateCurrentCornersRight.setPtr(new int(0));
	mCalibrateResultLeft.setPtr(new int(0));
	mCalibrateResultRight.setPtr(new int(0));
}

ThreadDataUnit<IplImage>& MainDataContainer::getImageLeftRef()
{
	return mLeftImage;
}

ThreadDataUnit<IplImage>& MainDataContainer::getImageRightRef()
{
	return mRightImage;
}

ThreadDataUnit<IplImage>& MainDataContainer::getImageLeftGrayRef()
{
	return mLeftImageGray;
}

ThreadDataUnit<bool>& MainDataContainer::drawChessboard()
{
	return mDrawChessboard;
}

ThreadDataUnit<short>& MainDataContainer::getCalibrateDataAccess()
{
	return mCalibrateDataAccess;
}

ThreadDataUnit<IplImage>& MainDataContainer::getImageRightGrayRef()
{
	return mRightImageGray;
}

ThreadDataUnit<CvPoint2D32f>& MainDataContainer::getCalibrateCurrentPointsLeft()
{
	return mCalibrateCurrentPointsLeft;
}

ThreadDataUnit<CvPoint2D32f>& MainDataContainer::getCalibrateCurrentPointsRight()
{
	return mCalibrateCurrentPointsRight;
}

ThreadDataUnit<int>& MainDataContainer::getCalibrateChessboardW()
{
	return mCalibrateChessboardW;
}

ThreadDataUnit<int>& MainDataContainer::getCalibrateChessboardH()
{
	return mCalibrateChessboardH;
}

ThreadDataUnit<int>& MainDataContainer::getCalibrateCurrentCornersLeft()
{
	return mCalibrateCurrentCornersLeft;
}

ThreadDataUnit<int>& MainDataContainer::getCalibrateCurrentCornersRight()
{
	return mCalibrateCurrentCornersRight;
}

ThreadDataUnit<int>& MainDataContainer::getCalibrateResultLeft()
{
	return mCalibrateResultLeft;
}

ThreadDataUnit<int>& MainDataContainer::getCalibrateResultRight()
{
	return mCalibrateResultRight;
}


MainDataContainer::~MainDataContainer()
{
	getImageLeftRef().lockData();
	if(getImageLeftRef().getPtr()!=NULL)
	{
		IplImage* tempImage		= getImageLeftRef().getPtr();
		cvReleaseData(tempImage);
		cvReleaseImage(&tempImage);
	}
	getImageLeftRef().unlockData();

	getImageRightRef().lockData();
	if(getImageRightRef().getPtr()!=NULL)
	{
		IplImage* tempImage		= getImageRightRef().getPtr();
		cvReleaseData(tempImage);
		cvReleaseImage(&tempImage);
	}
	getImageRightRef().unlockData();

	getImageLeftGrayRef().lockData();
	if(getImageLeftGrayRef().getPtr()!=NULL)
	{
		IplImage* tempImage		= getImageLeftGrayRef().getPtr();
		cvReleaseData(tempImage);
		cvReleaseImage(&tempImage);
	}
	getImageLeftGrayRef().unlockData();

	getImageLeftGrayRef().lockData();
	if(getImageRightGrayRef().getPtr()!=NULL)
	{
		IplImage* tempImage		= getImageRightGrayRef().getPtr();
		cvReleaseData(tempImage);
		cvReleaseImage(&tempImage);
	}
	getImageRightGrayRef().unlockData();

	mCalibrateCurrentPointsLeft.removePtr();
	mCalibrateCurrentPointsRight.removePtr();

	mCalibrateChessboardW.removePtr();
	mCalibrateChessboardH.removePtr();

	mCalibrateCurrentCornersLeft.removePtr();
	mCalibrateCurrentCornersRight.removePtr();

	mCalibrateResultLeft.removePtr();
	mCalibrateResultRight.removePtr();

	mDrawChessboard.removePtr();

}
