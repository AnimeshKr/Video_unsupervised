#include <cstdio>
#include <algorithm>
#include <cstring>
#include <cmath>
#include <vector>
#include <string>

using namespace std;


char filename[1010];
bool was[151][151];
int ct[151][151];

struct boxnode
{
	int x1,y1,x2,y2;
	int fid;
	float score;
};

int cmp(const boxnode &a, const boxnode &b)
{
	return a.score > b.score;
}

int main(int argc, char** argv)
{
	int startFrame, endFrame;
	int width, height;

	startFrame = 0;
	endFrame = 6000;
	height = 84;
	width = 150;

	int limitboxnum = 2000; //output max box nums

	char * filedir = argv[1];
	sprintf(filename, "%s/wh.txt", filedir);
	FILE * filewh = fopen(filename, "r");
	fscanf(filewh, "%d%d%d%d", &startFrame, &endFrame, &width, &height);
	fclose(filewh);

	float thres1 = 0.5; // whether it is a large step
	float thres2 = 0.75; // the portion of active points should not be more than thres2
	float thres3 = 0.25; // the portion of active points should not be less than thres3
	int slideH = 50;
	int slideW = 50;
	int slideStep = 8;
	vector<boxnode> bboxes;

	sprintf(filename, "%s/traj.txt", filedir);
	FILE * file = fopen(filename, "r");

	for (int i = startFrame; i < endFrame; i ++)
	{
		//printf("%d\n", i);
		//sprintf(filename, "%s/%d.txt", filedir, i);
		//FILE * file = fopen(filename, "r");
		int pnum;
		int x,y;
		float dx,dy;
		vector<int> xs,ys;
		vector<float> dxs,dys;
		memset(was, 0, sizeof(was));
		memset(ct, 0, sizeof(ct));

		fscanf(file, "%d", &pnum);
		for(int tp = 0; tp < pnum; tp ++)
		{
			//while(fscanf(file, "%d%d%f%f", &x,&y,&dx,&dy) > 0)
			fscanf(file, "%d%d%f%f", &x,&y,&dx,&dy);
			xs.push_back(x), ys.push_back(y);
			dxs.push_back(dx), dys.push_back(dy);
		}
		//fclose(file);

		int cnt1 = 0;
		for(int j = 0; j < dxs.size(); j ++)
		{
			float dis = dxs[j] * dxs[j] + dys[j] * dys[j];
			dis = sqrt(dis);
			if (dis  > thres1 )
			{
				was[ys[j]][xs[j]] = 1;
				cnt1 ++;
			}
		}

		if (cnt1 > dxs.size() * thres2  || cnt1 < dxs.size() * thres3) continue;

		//int mmn = -0;
		for(int j = 0; j < height; j ++)
			for(int k = 0; k < width; k ++)
			{
				if (j > 0 && k > 0)
				{
					ct[j][k] = was[j][k] + ct[j-1][k] + ct[j][k-1] - ct[j-1][k-1];
					//mmn = max(mmn, ct[j][k]);
					continue;
				}
				if (j == 0 && k == 0){ct[j][k] = was[j][k]; continue;}
				if (j == 0 && k > 0) {ct[j][k] = was[j][k] + ct[j][k-1]; continue;}
				if (j > 0  && k == 0){ct[j][k] = was[j][k] + ct[j-1][k]; continue;}
			}

		//printf("%d\n", mmn);
		float maxnum = -1e6;
		int recy = -1, recx= -1;
		int centerx = width / 2, centery = height / 2;
		for (int j = 1; j < height - slideH; j ++)
			for(int k = 1; k < width - slideW; k ++)
			{
				int y1 = j, x1 = k;
				int y2 = j + slideH - 1, x2 = k + slideW - 1;
				int cc = ct[y2][x2] - ct[y2][x1 - 1] - ct[y1 - 1 ][x2] + ct[y1 - 1][x1 - 1];
				int nowx = x1 + slideW / 2, nowy = y1 + slideH / 2;
				float dis = sqrt((float)((nowx - centerx) * (nowx - centerx) + (nowy - centery) * (nowy - centery)));
				if (maxnum < cc - dis )
				{
					maxnum = cc - dis ;
					recy = j, recx = k;
				}
			}
		boxnode tempbox;
		tempbox.x1 = recx;
		tempbox.y1 = recy;
		tempbox.x2 = recx + slideW;
		tempbox.y2 = recy + slideH;
		tempbox.fid = i;
		tempbox.score = maxnum;
		bboxes.push_back(tempbox);

	}

	fclose(file);



	sprintf(filename, "%s/bbox.txt", filedir);
	FILE * boxfile = fopen(filename, "w");
	//sort(bboxes.begin(), bboxes.end(), cmp);
	for (int i = 0; i < bboxes.size(); i ++)
	{
		fprintf(boxfile, "%d %d %d %d %d %f\n",bboxes[i].fid, bboxes[i].x1,  bboxes[i].y1,  bboxes[i].x2,  bboxes[i].y2, bboxes[i].score);
	}
	fclose(boxfile);

	// prune boxes

	sprintf(filename, "%s/prunebbox.txt", filedir);
	FILE * bb2 = fopen(filename, "w");
	int framegap = 10;
	int framelen = 30;
	int lowthres = 10, highthres = 100;

	int flag = 0; // startnew
	vector<int> vb;
	vector<int> rec;
	int numpbox = 0;
	for (int i = 0; i < bboxes.size(); i ++)
	{
		vb.clear();
		rec.clear();
		if (bboxes[i].score < lowthres || bboxes[i].score > highthres) continue;
		while(i < bboxes.size())
		{
			if(vb.size() == 0)
			{
				vb.push_back(bboxes[i].fid);
				rec.push_back(i);
				i ++;
				continue;
			}
			int nowfid = bboxes[i].fid;
			if (nowfid - vb[vb.size() - 1] > framegap )
			{
				break;
			}
			vb.push_back(nowfid);
			rec.push_back(i);
			i ++;
		}
		if (vb.size() < framelen) continue;

		for (int j = 0; j < rec.size(); j ++)
		{
			int id = rec[j];
			numpbox ++;
			fprintf(bb2, "%d %d %d %d %d %f %d\n",bboxes[id].fid, bboxes[id].x1,  bboxes[id].y1,  bboxes[id].x2,  bboxes[id].y2, bboxes[id].score, rec.size() - j);
		}
		fprintf(bb2, "\n");

		if(numpbox > limitboxnum )
			break;

	}

	fclose(bb2);

	return 0;
}


