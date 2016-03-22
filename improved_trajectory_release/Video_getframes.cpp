#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>
#include <ctype.h>
#include <unistd.h>

#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <list>
#include <string>

using namespace cv;

IplImage* image = 0; 
IplImage* prev_image = 0;
CvCapture* capture = 0;

int show = 0; 
int saveVideo = 1;
char filename[1010];

int main( int argc, char** argv )
{
	int frameNum = 0;

	char* video = argv[1];
	char* savePath = argv[2];
	capture = cvCreateFileCapture(video);

	if( !capture ) { 
		printf( "Could not initialize capturing..\n" );
		return -1;
	}
	
	if( show == 1 )
		cvNamedWindow( "Video", 0 );

	while( true ) {
		IplImage* frame = 0;
		int i, j, c;

		// get a new frame
		frame = cvQueryFrame( capture );
		if( !frame )
			break;

		if( !image ) {
			image =  cvCreateImage( cvSize(frame->width,frame->height), IPL_DEPTH_8U, 3 );
			image->origin = frame->origin;
		}

		cvCopy( frame, image, 0 );

		if( show == 1 ) {
			cvShowImage( "Video", image);
			c = cvWaitKey(3);
			if((char)c == 27) break;
		}
		
		if(saveVideo == 1 /*&& frameNum >= 1000*/)
		{
			Mat mtx(image);
			//Mat mtx2(frame->height, frame->width, CV_32FC3);
			//mtx.convertTo(mtx, CV_32FC3);
			//Mat_<Vec3b> _I = mtx;
			Mat mtx2;
			//cv::resize(mtx, mtx2, cv::Size(150, 84));
			int maxlen = min(mtx.rows, mtx.cols); 
			float ratio = maxlen * 1.0 / 256.0;
			int height = mtx.rows / ratio;
			int width  = mtx.cols / ratio;  
			cv::resize(mtx, mtx2, cv::Size(width, height));
			sprintf(filename, "%s/%04d.jpg", savePath, frameNum);
			//printf("%s\n", filename);
			imwrite(filename, mtx2);

			/*Mat mtx2;
			mtx2 = imread(filename);
			sprintf(filename, "%s/%d.jpg", savePath, frameNum);
			printf("%s\n", filename);
			imwrite(filename, mtx2);*/
			//cvSaveImage(filename, image, 0);
		}


//		std::cerr << "The " << frameNum << "-th frame" << std::endl;
		frameNum++;
		//if(frameNum == 3000)
		//	break;
	}

	if( show == 1 )
		cvDestroyWindow("Video");

	return 0;
}
