
#include <iostream>
#include <fstream>
#include <cstdio>
#include <algorithm>
#include <string>
#include <cstring>
#include <vector>

#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>

#include <math.h>
#include <list>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>
#include <ctype.h>
#include <unistd.h>


using namespace cv;
using namespace std;

string listsrc  = "/home/xiaolonw/video_dataset/tempvideos/temp_videos_3/list/";
string videosrc = "/home/xiaolonw/video_dataset/tempvideos/temp_videos_3/videos/";
string saveDir = "/home/xiaolonw/video_dataset/tempvideos/temp_videos_3/frames/";
char fname[1010];
char actname[1010], vname[1010];
char numname1[1010], numname2[1010];
char videoname[1010];
char savename[1010];
char savename2[1010];
char savename3[1010];
char lockname[1010];

int str2num(string num)
{
	if(!(num[0] >= '0' && num[0] <= '9') )
		return -1;
	if(!(num.size() == 8 && num[2] == ':' && num[5] == ':') )
		return -1;
	if(num.size() == 0)
		return -2;
        for (int i = 0; i < num.size(); i ++) num[i] = num[i] - '0';
	int val = (num[0] * 10 + num[1]) * 3600 + (num[3] * 10 + num[4]) * 60 + (num[6] * 10 + num[7]);
	return val;
}


int extractframes(char * videoname, vector<int> frameids, char * savePath)
{

	IplImage* image = 0; 
	CvCapture* capture = 0;
	int frameNum = 1;
	char tsave[1010], filename[1010];

	capture = cvCreateFileCapture(videoname);

	if( !capture ) { 
		printf( "Could not initialize capturing..\n" );
		return -1;
	}

	int clipnum = 0;
	float currenttime = cvGetCaptureProperty(capture, CV_CAP_PROP_POS_MSEC); 
        // debug
        for (int i = 0; i < frameids.size(); i ++) 
	{
	    printf("%d ", frameids[i]); 
	} 
	printf("\n"); 
        
	//
	while( clipnum * 2 < frameids.size() ) 
	{
		IplImage* frame = 0;

		int starttime = frameids[clipnum * 2] * 1000;
		int endtime   = frameids[clipnum * 2 + 1] * 1000;
                if (frameNum % 100 == 0)
	            printf("framenum %d, currenttime: %f\n", frameNum, currenttime);  
		if (starttime > currenttime) 
		{
			frameNum ++;
			currenttime = cvGetCaptureProperty(capture, CV_CAP_PROP_POS_MSEC);
			frame = cvQueryFrame( capture );
			if( !frame )
				break;
			continue; 
		}

		int flag = 0;
	        printf("starttime: %d,  currenttime: %f\n", starttime, currenttime);  
		while (currenttime <= endtime) 
		{

			// get a new frame
			frame = cvQueryFrame( capture );
			currenttime = cvGetCaptureProperty(capture, CV_CAP_PROP_POS_MSEC);
			if( !frame )
			{
				flag = 1;
				break;
			}

			if( !image ) 
			{
				image =  cvCreateImage( cvSize(frame->width,frame->height), IPL_DEPTH_8U, 3 );
				image->origin = frame->origin;
			}

			cvCopy( frame, image, 0 );
			
			Mat mtx(image);
			Mat mtx2;
			int maxlen = min(mtx.rows, mtx.cols); 
			float ratio = maxlen * 1.0 / 256.0;
			int height = mtx.rows / ratio;
			int width  = mtx.cols / ratio;  
			cv::resize(mtx, mtx2, cv::Size(width, height));

			sprintf(tsave, "%s/%d", savePath, clipnum);
			DIR * tempd2 = opendir(tsave);
			if (tempd2 == NULL)
			{
				mkdir(tsave, 0777);
			}
			else 
				closedir(tempd2);

			sprintf(filename, "%s/%04d.jpg", tsave, frameNum);
			imwrite(filename, mtx2);

			frameNum++;


		}
		clipnum ++; 

		if (flag == 1) break;


	}

	return 1;


}



int main( int argc, char** argv)
{
    DIR *dp, *dpv, *dpimg;
    struct dirent *dirp;

    dp = opendir(listsrc.c_str());

    if (dp == NULL)
    {
        cout << "Error opening " << listsrc << endl;
        return 1;
    }
    while ((dirp = readdir( dp )))
    {
        string sequence =dirp->d_name;
        if(strcmp(sequence.c_str(), ".") ==0  || strcmp(sequence.c_str(), "..") ==0)
            continue;

        string vdir = listsrc + "/" + sequence;
        dpv = opendir(vdir.c_str());
        if (dpv == NULL)
            continue;

        string listfile = vdir + "/Sheet1.txt"; 
        FILE * fid = fopen(listfile.c_str(), "r");

        for(int i = 0; i < 29; i ++ )
        {
        	fscanf(fid, "%s", fname); 
        }

        int flag = 0;
        while(true)
        {
        	fscanf(fid, "%s", actname);
			int actnum = str2num(actname); 
	        if (actname[0] >= '0' && actname[0] <= '9' )
			{
				flag = 1;
				break;
			}
        	fscanf(fid, "%s", vname);
        	vector<int> que; 
        	while(true)
        	{
        		fscanf(fid, "%s", numname1);
        		string tnum = numname1; 
        		int num1 = str2num(tnum);
        		/*if (num1 < 0) 
        		{
        			break;
        		}*/
        		int tt = fscanf(fid, "%s", numname2); 
        		if (tt <= 0)
        		{
        			flag = 1;
        			break;
        		} 		
        		tnum = numname2; 
        		int num2 = str2num(tnum);
        		if(num2 < 0)
        		{
        			break;
        		}
        		que.push_back(num1);
        		que.push_back(num2);
      
        	}

        	sprintf(videoname, "%s/%s/%s.mp4", videosrc.c_str(), actname, vname);
        	sprintf(savename, "%s/%s", saveDir.c_str(), sequence.c_str());
        	DIR * tempd = opendir(savename);
			if (tempd == NULL)
			{
				mkdir(savename, 0777);
			}
			else 
				closedir(tempd);
			sprintf(savename2, "%s/%s", savename, actname);
			DIR * tempd2 = opendir(savename2);
			if (tempd2 == NULL)
			{
				mkdir(savename2, 0777);
			}
			else
				closedir(tempd2);
			sprintf(savename3, "%s/%s.mp4", savename2, vname);
			DIR * tempd3 = opendir(savename3);
			if (tempd3 == NULL)
			{
				mkdir(savename3, 0777);
			}
			else
				closedir(tempd3);

			sprintf(lockname, "%s/%s.lock", savename2, vname);
			DIR * tempd4 = opendir(lockname);
			if (tempd4 == NULL)
			{
				mkdir(lockname, 0777);
			}
			else
			{
				printf("lock: %s\n", lockname);
				closedir(tempd4);
				if(flag == 1) break;
				continue;
			}

			printf("%s %s\n", videoname, savename3);
			int res = extractframes(videoname, que, savename3);

        	if(flag == 1) 
        	{
	        	printf("\n");
        		break;
        	}

        }



        fclose(fid); 




    }
    return 0;


}













