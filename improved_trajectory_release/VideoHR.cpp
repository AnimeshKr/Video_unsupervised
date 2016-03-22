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

#include <map>

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;
using namespace cv;

IplImage* image = 0; 
IplImage* prev_image = 0;
CvCapture* capture = 0;

int show = 0;
int saveVideo = 1;
int resizeLen = 600;
char filename[1010];
char trackfilename[1010];
char imname1[1010];
char imname2[1010];
char cmdname[1010];
char savepairs[1010];



string makedir(char *sdir)
{
    string front = "/home/xiaolonw/videos/data/YouTube/collect/";
    string nowdir = "/home/xiaolonw/videos/data/pairs/collect/";
    int flen = front.size();
    int slen = strlen(sdir);
    for (int i = flen; i < slen; i ++)
    {
	int j = i;
	while(j < slen && sdir[j] != '/')
	{
	    nowdir = nowdir + sdir[j];
	    j++;
	}
	nowdir = nowdir + sdir[j];
	i = j;
	DIR * tempd = opendir(nowdir.c_str());
	if (tempd == NULL) 
	{
		mkdir(nowdir.c_str(), 0777); 
	}
	else
		closedir(tempd);
	
    }
    return nowdir;
}



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

	if (prunetxt == NULL)
		return 0;

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

	if(saveVideo == 1)
	{
		sprintf(filename, "%s/imgshr", savePath);
		DIR * tempd = opendir(filename);
		if (tempd == NULL)
		{
			mkdir(filename, 0777);
		}
		else
			closedir(tempd);
	}

	sprintf(trackfilename, "%s/trackbbox2.txt", savePath);
	FILE * trackfile = fopen(trackfilename , "r");
	float trackscale = 1;
	int id1, id2, rw, rh;
	int x1, y1, x2, y2;
	int bx1, by1, bx2, by2;

	map<int,int> mp;
	mp.clear();

	if (trackfile == NULL)
	{
		return 0;
	}

	while(fscanf(trackfile, "%d%d", &id1, &id2) > 0)
	{
		mp[id1] = 1;
		mp[id2] = 1;
		fscanf(trackfile, "%d%d", &rw, &rh);
		fscanf(trackfile, "%d%d%d%d", &x1, &y1, &x2, &y2);
		fscanf(trackfile, "%d%d%d%d", &bx1, &by1, &bx2, &by2);
	}
	
	fclose(trackfile);

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

		
		if(saveVideo == 1 && mp.find(frameNum) != mp.end()) 
		{
			Mat mtx(image);
			Mat mtx2;
			cv::resize(mtx, mtx2, cv::Size(resizeW, resizeH));
			sprintf(filename, "%s/imgshr/%07d.jpg", savePath, frameNum);
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

	//sprintf(filename, "%s/pairs", savePath);
	sprintf(filename, "%s/pairs2", savePath);
	DIR * tempd2 = opendir(filename);
	if (tempd2 == NULL)
	{
		mkdir(filename, 0777);
	}
	else
		closedir(tempd2);

	//sprintf(filename, "%s/trackbbox.txt", savePath);
	// sprintf(filename, "%s/trackbbox2.txt", savePath);
	

	trackfile = fopen(trackfilename , "r");
	int cropSize = 227;
	int cropcnt = 0;

	if (trackfile == NULL)
	{
		sprintf(cmdname, "rm -rf %s/imgshr", savePath);
		system(cmdname);
		return 0;
	}
	while(fscanf(trackfile, "%d%d", &id1, &id2) > 0)
	{
		cropcnt ++;
		sprintf(imname1, "%s/imgshr/%07d.jpg", savePath, id1);
		sprintf(imname2, "%s/imgshr/%07d.jpg", savePath, id2);
		Mat im1 = imread(imname1, CV_LOAD_IMAGE_COLOR);
		Mat im2 = imread(imname2, CV_LOAD_IMAGE_COLOR);
		fscanf(trackfile, "%d%d", &rw, &rh);
		fscanf(trackfile, "%d%d%d%d", &x1, &y1, &x2, &y2);
		fscanf(trackfile, "%d%d%d%d", &bx1, &by1, &bx2, &by2);
		if (im1.data == NULL || im2.data == NULL)
		{
			continue;
		}
		float nowscale = (resizeH * 1.0) / (rh * 1.0);
		x1 = max((int)(x1 * nowscale), 0);
		y1 = max((int)(y1 * nowscale), 0);
		x2 = min((int)(x2 * nowscale), resizeW - 1);
		y2 = min((int)(y2 * nowscale), resizeH - 1);
		bx1 = bx1 * nowscale;
		by1 = by1 * nowscale;
		bx2 = bx2 * nowscale;
		by2 = by2 * nowscale;
		int nbx1 = max((int)(bx1 ), 0);
		int nby1 = max((int)(by1 ), 0);
		int nbx2 = min((int)(bx2 ), resizeW - 1);
		int nby2 = min((int)(by2 ), resizeH - 1);
		int sbx1 = nbx1 - bx1;
		int sby1 = nby1 - by1;
		int sbx2 = bx2 - nbx2;
		int sby2 = by2 - nby2;

		int nowW = x2 - x1 + 1;
		int nowH = y2 - y1 + 1;

		Mat cropim1(Size(nowW, nowH), CV_8UC3);
		Mat cropim2(Size(nowW, nowH), CV_8UC3);

		for (int c = 0; c < 3; ++c) {
			for (int h = 0; h < nowH; ++h) {
			  for (int w = 0; w < nowW; ++w) {
				  cropim1.at<cv::Vec3b>(h, w)[c] = im1.at<cv::Vec3b>(h + y1, w + x1)[c];
			  }
			}
		}
		for (int c = 0; c < 3; ++c) {
			for (int h = 0; h < nowH; ++h) {
			  for (int w = 0; w < nowW; ++w) {
				  cropim2.at<cv::Vec3b>(h, w)[c] = 0;
			  }
			}
		}
		int nowW2 = nbx2 - nbx1 + 1;
		int nowH2 = nby2 - nby1 + 1;
		for (int c = 0; c < 3; ++c) {
			for (int h = 0; h < nowH2; ++h) {
			  for (int w = 0; w < nowW2; ++w) {
				  if (h + sby1 >= nowH || w + sbx1 >= nowW) continue;
				  if (h + nby1 >= resizeH || w + nbx1 >= resizeW) continue;
				  cropim2.at<cv::Vec3b>(h + sby1, w + sbx1)[c] = im2.at<cv::Vec3b>(h + nby1, w + nbx1)[c];
			  }
			}
		}
		//sprintf(imname1, "%s/pairs/%07d_1.jpg", savePath, cropcnt);
		//sprintf(imname2, "%s/pairs/%07d_2.jpg", savePath, cropcnt);

                sprintf(savepairs, "%s/pairs2/", savePath);
		string sp = makedir(savepairs);
		sprintf(imname1, "%s/%07d_1.jpg", sp.c_str(), cropcnt);
		sprintf(imname2, "%s/%07d_2.jpg", sp.c_str(), cropcnt);
		
                Mat resizedCropim1(Size(cropSize, cropSize), CV_8UC3);
		Mat resizedCropim2(Size(cropSize, cropSize), CV_8UC3);
		resize(cropim1, resizedCropim1, cv::Size(cropSize, cropSize));
		resize(cropim2, resizedCropim2, cv::Size(cropSize, cropSize));

		imwrite(imname1, resizedCropim1);
		imwrite(imname2, resizedCropim2);



	}
	fclose(trackfile);

	sprintf(cmdname, "rm -rf %s/imgshr", savePath);
	system(cmdname);




	return 0;
}







