#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "find_head.h"
#include "cameraParameters.h"
#include "initialization.h"
#include "paramsCalculate.h"
#include "kalman.h"
#include "getHeightmap.h"

using namespace std;
using namespace cv;

int main()
{
	double array2[16] = { 100, 0, 0, 0, 0, 100, 0, 0, 0, 0, 100, 0, 0, 0, 0, 100 };
	Mat P = Mat(4, 4, CV_64F, array2);
	cameraParameters *params = new cameraParameters;
	params->b = 15.97;
	params->f = 172;
	params->u0 = 121;
	params->v0 = 84;

	int d_range = 512;
	int rows = 177;
	int cols = 244;
	float groundDepth = 290;

	float** ProjectMatrixHeight = new float *[rows*cols];
	int** ProjectMatrixX = new int *[rows*cols];
	for (int i = 0; i < rows*cols; i++)
	{
		ProjectMatrixHeight[i] = new float[d_range];
		ProjectMatrixX[i] = new int[d_range];
	}

	initialization(rows, cols, d_range, params, groundDepth, 255, ProjectMatrixX, ProjectMatrixHeight);

	int th_height1 = 180;
	int th_height2 = 190;
	int th_height3 = 220;

	double th_Area1 = 230;//control noise
	double th_Area2 = 190;//control further segment

	for (int n = 935; n <= 978; n++)
	{
		cout << "µÚ" << n << "Ö¡:";
		char imgname[20];
		sprintf_s(imgname, "Img7fgMask%05d.png", n);
		Mat origin_img = imread(imgname, 0);
		Mat getHead(origin_img.rows, origin_img.cols, CV_8U, Scalar::all(0));
		Mat heightmap_d(255, 255, CV_8U, Scalar::all(0));
		Mat heightmap_head(255, 255, CV_8U, Scalar::all(0));

		/*namedWindow("origin_img");
		imshow("origin_img", origin_img);
		waitKey(1);*/
		//double t = getTickCount();
		getHeightmap(origin_img, heightmap_d, ProjectMatrixX, ProjectMatrixHeight);//heightmap of fg
		find_head(th_height1, th_height2, th_height3, th_Area1, th_Area2, origin_img, getHead);
		getHeightmap(getHead, heightmap_head, ProjectMatrixX, ProjectMatrixHeight);//heightmap of found head
		//t = getTickCount() - t;
		//cout << t * 1000 / getTickFrequency() << endl;
		namedWindow("head");
		imshow("head", getHead);
		waitKey(1);

		Obj object;
		
		paramsCalculate(heightmap_d, heightmap_head, object);
		
		static Obj last_obj;
		
		kalman(last_obj, object, P);
		
		cout << "ÈËÊý " << last_obj.get_length()<< endl;
	}
	return 0;
}