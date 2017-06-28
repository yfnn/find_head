#ifndef _CAMERAPARAMETERS_H_
#define _CAMERAPARAMETERS_H_

#include <opencv2/core/core.hpp>
using namespace cv;

struct cameraParameters
{
	float b;
	float f;
	ushort u0;
	ushort v0;
};

#endif