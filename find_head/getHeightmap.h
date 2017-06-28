#ifndef _GETHEIGHTMAP_H_
#define _GETHEIGHTMAP_H_
#include <opencv2/opencv.hpp>
using namespace cv;

void getHeightmap(Mat d, Mat& heightmap, int** ProjectMatrixX, float** ProjectMatrixHeight);
#endif