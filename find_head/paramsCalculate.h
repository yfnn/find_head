#ifndef _PARAMSCALCULATE_H_
#define _PARAMSCALCULATE_H_
#include <opencv2/core/core.hpp>
#include "object.h"
void paramsCalculate(Mat getHead, Obj& object, int** ProjectMatrixX, float** ProjectMatrixHeight);
#endif