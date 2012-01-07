
#include "MainDataContainer.h"

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

ThreadDataUnit<IplImage>& MainDataContainer::getImageRightGrayRef()
{
	return mRightImageGray;
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

}
