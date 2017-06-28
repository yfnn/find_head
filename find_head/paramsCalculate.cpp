#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "object.h"
#include "getHeightmap.h"
using namespace cv;
using namespace std;

void paramsCalculate(Mat getHead, Obj& object, int** ProjectMatrixX, float** ProjectMatrixHeight)
{
	int x, y;
	int w, h;
	float height;

	Mat heightmap(255, 255, CV_8U, Scalar::all(0));
	getHeightmap(getHead, heightmap, ProjectMatrixX, ProjectMatrixHeight);
	/*namedWindow("hm");
	imshow("hm", heightmap);
	waitKey(1);*/
	Mat height_t;
	heightmap.copyTo(height_t);

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(height_t, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	for (int i = 0; i < contours.size(); i++)
	{
		double tempArea = contourArea(contours[i]);
		if (tempArea > 10)
		{
			Moments mom = moments(Mat(contours[i]));
			x = mom.m10 / mom.m00;
			y = mom.m01 / mom.m00;
			x = (x - 127) * 3;
			y = (y - 127) * 3;
			Rect rect = boundingRect(contours[i]);
			w = 40;
			h = 40;
			double max_h;
			Point maxIdx;
			Mat p = heightmap(rect);
			minMaxLoc(p, NULL, &max_h, NULL, &maxIdx);
			height = max_h;
			object.push(x, y, w, h, height);
		}
	}
}