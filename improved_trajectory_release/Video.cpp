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
int resizeLen = 150;
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

	sprintf(filename, "%s/prunebbox.txt", savePath);
	FILE * prunetxt = fopen(filename, "r");
	int num;
	float temp;
	vector<int> ids;
	while(fscanf(prunetxt, "%d", &num) > 0)
	{
		ids.push_back(num);
		for(int i = 0; i < 4; i ++) fscanf(prunetxt, "%d", &num);
		fscanf(prunetxt, "%f", &temp);
		fscanf(prunetxt, "%d", &num);
	}
	fclose(prunetxt);

	int cnt = 0;
	int resizeH, resizeW;
	float scal;


	while( true ) {
		IplImage* frame = 0;
		int i, j, c;

		// get a new frame
		frame = cvQueryFrame( capture );
		if( !frame )
			break;

		if (cnt >= ids.size() )
		{
			frameNum ++;
			continue;
		}
		if(cnt > 0 && ids[cnt - 1] < frameNum && frameNum < ids[cnt] && ids[cnt] - ids[cnt-1] > 10)
		{
			frameNum ++;
			continue;
		}
		if (frameNum == ids[cnt])
		{
			cnt ++;
		}
		if (cnt == 0 )
		{
			frameNum ++;
			continue;
		}


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
		int maxlen = max(frame->width,frame->height);
		scal  = resizeLen * 1.0 / maxlen;
		resizeH = frame->height * scal;
		resizeW = frame->width  * scal;

		
		if(saveVideo == 1 /*&& frameNum >= 1000*/)
		{
			Mat mtx(image);
			//Mat mtx2(frame->height, frame->width, CV_32FC3);
			//mtx.convertTo(mtx, CV_32FC3);
			//Mat_<Vec3b> _I = mtx;
			Mat mtx2;
			cv::resize(mtx, mtx2, cv::Size(resizeW, resizeH));
			sprintf(filename, "%s/imgs/%07d.jpg", savePath, frameNum);
			//printf("%s\n", filename);
			imwrite(filename, mtx2);

		}


		//std::cerr << "The " << frameNum << "-th frame" << std::endl;
		frameNum++;
		//if(frameNum == 3000)
		//	break;
	}

	if( show == 1 )
		cvDestroyWindow("Video");


    sprintf(filename, "%s/wh2.txt", savePath);
	FILE * filewh = fopen(filename, "w");
	fprintf(filewh, "%d %d\n", resizeW, resizeH);
	fclose(filewh);


	// alter prunebbox file
	int startFrame, endFrame, width, height;
	sprintf(filename, "%s/wh.txt", savePath);
	filewh = fopen(filename, "r");
	fscanf(filewh, "%d%d%d%d", &startFrame, &endFrame, &width, &height);
	fclose(filewh);



	sprintf(filename, "%s/prunebbox.txt", savePath);
	prunetxt = fopen(filename, "r");
	sprintf(filename, "%s/prunebbox2.txt", savePath);
	FILE * prunetxt2 = fopen(filename, "w");
	while(fscanf(prunetxt, "%d", &num) > 0)
	{
		fprintf(prunetxt2, "%d ", num);
		for(int i = 0; i < 4; i ++)
		{
			fscanf(prunetxt, "%d", &num);
			num = num * (resizeH * 1.0 / height );
			fprintf(prunetxt2, "%d ", num);
		}
		fscanf(prunetxt, "%f", &temp);
		fprintf(prunetxt2, "%f ", temp);
		fscanf(prunetxt, "%d", &num);
		fprintf(prunetxt2, "%d\n", num);
	}

	fclose(prunetxt);
	fclose(prunetxt2);




	return 0;
}







