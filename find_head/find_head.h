#ifndef _FIND_HEAD_H_
#define _FIND_HEAD_H_

#include <opencv2/core/core.hpp>
using namespace cv;
void find_head(int th_height1, int th_height2, int th_height3, double th_Area1, double th_Area2, Mat origin_img, Mat& getHead);
#endif