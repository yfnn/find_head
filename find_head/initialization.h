#ifndef _INITIALIZATION_
#define _INITIALIZATION_
#include <opencv2/core/core.hpp>
#include "cameraParameters.h"
using namespace cv;

void initialization(int rows, int cols, int d_range, cameraParameters *params, short groundDepth, int heightmap_width, int**& ProjectMatrixX, float**& ProjectMatrixHeight)
{
	float b = params->b;
	float f = params->f;
	ushort u0 = params->u0;
	ushort v0 = params->v0;
	float x, y, z;

	float coreX = groundDepth*((cols + 1) / 2 - u0) / f;//以图像中心的3D x,y为最终场景的中心，将其放在heightMap（128，,128）的位置
	float coreY = groundDepth*((rows + 1) / 2 - v0) / f;

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			for (int d = 1; d <= d_range; d++)
			{
				z = b*f*16 / d;
				x = z*(j - u0 + 1) / f;
				y = z*(i - v0 + 1) / f;

				if (z > groundDepth) ProjectMatrixHeight[i*cols + j][d - 1] = 0;
				else ProjectMatrixHeight[i*cols + j][d - 1] = groundDepth - z;
				int x1 = int((x - coreX) / 3 + (heightmap_width - 1) / 2);
				int y1 = int((y - coreY) / 3 + (heightmap_width - 1) / 2);
				if (x1 >= 0 && y1 >= 0 && x1 <= heightmap_width - 1 && y1 <= heightmap_width - 1)
					ProjectMatrixX[i*cols + j][d - 1] = x1 + y1*heightmap_width;
				else
					ProjectMatrixX[i*cols + j][d - 1] = 0;
			}
		}
	}
}
#endif