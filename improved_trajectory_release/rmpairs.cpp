#include <fstream>
#include <iostream>
#include <string>
#include <cstring>

#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>

using namespace std;

//string videosrc = "/home/xiaolonw/videos/data/YouTube/general";
string videosrc = "/home/xiaolonw/videos/data/YouTube/collect";
string filepath;

int main()
{
	ifstream fin;
	DIR *dp, *dpv;
	struct dirent *dirp, *dirpv;

	//string cmdenv1 = "export LD_LIBRARY_PATH=/usr/local/lib/";
	//system(cmdenv1.c_str());

//	string cmdenv1="echo \$LD_LIBRARY_PATH";
//	system(cmdenv1.c_str());

	dp = opendir( videosrc.c_str() );
	if (dp == NULL)
	{
		cout << "Error opening " << videosrc << endl;
		return 1;
	}
	int cnt = 0;

	while ((dirp = readdir( dp )))
	{
		string sequence =dirp->d_name;
		if(strcmp(sequence.c_str(), ".") ==0  || strcmp(sequence.c_str(), "..") ==0)
			continue;
		string vdir = videosrc + "/" + sequence + "/videos";
		dpv = opendir(vdir.c_str());
		if (dpv == NULL) 
		    continue;
		string save_vdir = videosrc + "/" + sequence + "/pairs_traj";

		DIR * tempd = opendir(save_vdir.c_str());
		if (tempd == NULL)
		{
			mkdir(save_vdir.c_str(), 0777);
		}
		else
			closedir(tempd);


		while ((dirpv = readdir( dpv )))
		{
			cnt ++;
			string vname = dirpv->d_name;
			if(strcmp(vname.c_str(), ".") ==0  || strcmp(vname.c_str(), "..") ==0)
						continue;
			string videoname = vdir + "/" + vname;
			string vfname = vname.substr(0,vname.size() - 4);

			string pairdir = save_vdir + "/" + vfname;

			string lockdir = pairdir + "_lock_wxl";
			rmdir(lockdir.c_str());
			if(cnt % 1000 == 0)
			printf("%d\n", cnt);
		}

		closedir(dpv);

	}

	closedir(dp);

	return 0;
}
