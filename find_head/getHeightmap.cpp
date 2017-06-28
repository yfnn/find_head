#include <opencv2/opencv.hpp>

using namespace cv;

void getHeightmap(Mat d, Mat& heightmap, int** ProjectMatrixX, float** ProjectMatrixHeight){
	uchar* hm = heightmap.data;
	uchar* p_d = d.data;
	for (int n = 0; n < (d.cols*d.rows); n++)
	{
		if (p_d[n] != 0)
		{
			int p3d = ProjectMatrixX[n][p_d[n] + 128 - 1];
			if (hm[p3d]<ProjectMatrixHeight[n][p_d[n] + 128 - 1])
				hm[p3d] = uchar(ProjectMatrixHeight[n][p_d[n] + 128 - 1]);
		}
	}
}