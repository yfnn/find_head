#ifndef _KALMAN_
#define _KALMAN_

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <cmath>
#include "object.h"
using namespace cv;
using namespace std;
double array0[16] = { 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1 };
double array1[8] = { 1, 0, 0, 0, 0, 1, 0, 0 };

Mat A = Mat(4, 4, CV_64F, array0);
Mat H = Mat(2, 4, CV_64F, array1);

static Mat Q = Mat::eye(4, 4, CV_64F);
static Mat R = Mat::eye(2, 2, CV_64F);

//int find_pos(int* temp, int val, int length)
//{
//	for (int i = 0; i < length; i++)
//	{
//		if (temp[i] == val)
//		{
//			return i;
//		}
//	}
//	return length;
//}
int find_pos(int* temp, int length)
{
	for (int i = 0; i < length; i++)
	{
		if (temp[i] == 0)
		{
			return i + 1;
		}
	}
	return length;
}

void kalman(Obj& last_obj, Obj& cur_obj, Mat& P)
{
	Obj pre_obj;
	/*if (last_obj.noObject())
	{
	last_obj = cur_obj;
	cur_obj.empty();
	}*/
	//else if (!last_obj.noObject() && cur_obj.noObject()) last_obj.empty();
	/*else
	{*/
	P = A*P*A.t() + Q;
	Mat kf_gain;
	/*Mat kf_gain = P*H.t()/(H*P*H.t()+R);*/
	Mat t;
	invert(H*P*H.t() + R, t, CV_SVD);
	kf_gain = P*H.t()*t;
	Mat x = Mat(4, 1, CV_64F);
	int last_num = last_obj.get_length();
	int cur_num = cur_obj.get_length();
	for (int i = 0; i < last_num; i++)
	{
		x.at<double>(0, 0) = last_obj[i]->X;
		x.at<double>(1, 0) = last_obj[i]->Y;
		x.at<double>(2, 0) = last_obj[i]->Vx;
		x.at<double>(3, 0) = last_obj[i]->Vy;
		x = A*x;
		last_obj[i]->X = x.at<double>(0, 0);
		last_obj[i]->Y = x.at<double>(1, 0);
	}
	/*int* temp = new int[cur_num + 1];
	for (int i = 0; i < cur_num; i++)
	{
	temp[i] = i + 1;
	}*/

	Mat d_sm(cur_num, last_num, CV_64F, Scalar::all(0));//define similarity matrix

	for (int i = 0; i < cur_num; i++)//匹配算法
	{
		for (int j = 0; j < last_num; j++)
		{
			//double value = imsimilar(last_obj[i].hist, cur_obj[j].hist, 2);//判断两个直方图的相似性
			//double d = (min(last_obj[i]->X, cur_obj[j]->X) / max(last_obj[i]->X, cur_obj[j]->X))*(min(last_obj[i]->Y, cur_obj[j]->Y) / max(last_obj[i]->Y, cur_obj[j]->Y));//位置的相似性
			double d = sqrt((last_obj[j]->X - cur_obj[i]->X)*(last_obj[j]->X - cur_obj[i]->X) + (last_obj[j]->Y - cur_obj[i]->Y)*(last_obj[j]->Y - cur_obj[i]->Y));
			d_sm.at<double>(i, j) = d;
			//double thr = d * 100;
			//if (d <= 10)//thr>=50
			//{
			//	last_obj[i]->behind_match = j + 1;
			//	int index = find_pos(temp, last_obj[i]->No, cur_num);
			//	temp[index] = cur_obj[j]->No;
			//	temp[j] = last_obj[i]->No;
			//}
			/*else
			{
			if (value >= 0.75)
			{
			last_obj[i]->behind_match = j + 1;
			int index = find_pos(temp, last_obj[i]->No, cur_num);
			temp[index] = cur_obj[j]->No;
			temp[j] = last_obj[i]->No;
			}
			}*/
		}
	}

	int search_cnt = last_num < cur_num ? last_num : cur_num;
	int No_list[20];//20 reserved object Nos,0:unused,1:used
	for (int i = 0; i<20; i++)
		No_list[i] = 0;
	for (int i = 0; i < search_cnt; i++)//search for best association
	{
		double d_min = 0;
		Point d_loc;
		minMaxLoc(d_sm, &d_min, NULL, &d_loc, NULL);
		if (d_sm.at<double>(d_loc.y, d_loc.x) <= 30 && cur_obj[d_loc.y]->forward_match == -1 && last_obj[d_loc.x]->behind_match == -1)//one good match
		{
			cur_obj[d_loc.y]->forward_match = d_loc.x;
			cur_obj[d_loc.y]->No = last_obj[d_loc.x]->No;
			No_list[cur_obj[d_loc.y]->No - 1] = 1;
			last_obj[d_loc.x]->behind_match = d_loc.y;
		}
		else if (d_sm.at<double>(d_loc.y, d_loc.x) <= 30 && (cur_obj[d_loc.y]->forward_match != -1 || last_obj[d_loc.x]->behind_match != -1))
		{
			i--;
		}
		else{}
		d_sm.at<double>(d_loc.y, d_loc.x) = 500;

	}


	/*for (int i = 0; i < cur_num; i++)
	{
	cur_obj[i]->No = temp[i];
	}*/

	int last_cnt = 0;
	for (int i = 0; i<last_num; i++)//miss detection && left object process
	{
		if (last_obj[i]->behind_match == -1)
		{
			if (last_obj[i]->undetected_cnt <= 1 && last_obj[i]->X >= -110 && last_obj[i]->X <= 110 && last_obj[i]->Y >= -70 && last_obj[i]->Y <= 70)
			{
				cur_obj.push(last_obj[i]->X, last_obj[i]->Y, last_obj[i]->W, last_obj[i]->H, last_obj[i]->height);
				cur_obj[cur_num + last_cnt]->Vx = last_obj[i]->Vx;
				cur_obj[cur_num + last_cnt]->Vy = last_obj[i]->Vy;
				cur_obj[cur_num + last_cnt]->No = last_obj[i]->No;
				//cur_obj[cur_num + last_cnt]->behind_match = -1;
				//cur_obj[cur_num + last_cnt]->forward_match = -1;
				No_list[last_obj[i]->No - 1] = 1;
				cur_obj[cur_num + last_cnt]->undetected_cnt = last_obj[i]->undetected_cnt + 1;
				//cur_obj[cur_num + last_cnt]->forward_match= 0;
				last_cnt++;
			}
			//else
			//{
			//No_list[last_obj[i]->No - 1] = 0;
			//}
		}
	}

	for (int i = 0; i<cur_num; i++)//new object process
	{
		//cur_obj[i]->forward_match = -1;
		if (cur_obj[i]->forward_match == -1)//当前帧检测目标没有匹配
		{
			cur_obj[i]->No = find_pos(No_list, 20);
			No_list[cur_obj[i]->No - 1] = 1;
			if (cur_obj[i]->X>0)//从右边进来
				cur_obj[i]->Vx = -6;
		}
	}
	pre_obj = cur_obj;
	for (int i = 0; i < cur_num; i++)
	{
		int index = cur_obj[i]->forward_match;
		if (index != -1)
		{
			Mat _t = Mat(4, 1, CV_64F);
			Mat _s = Mat(2, 1, CV_64F);
			_t.at<double>(0, 0) = last_obj[index]->X;
			_t.at<double>(1, 0) = last_obj[index]->Y;
			_t.at<double>(2, 0) = last_obj[index]->Vx;
			_t.at<double>(3, 0) = last_obj[index]->Vy;
			_s.at<double>(0, 0) = cur_obj[i]->X;
			_s.at<double>(1, 0) = cur_obj[i]->Y;

			Mat z = H * _t;
			Mat r = _s - z;
			x = _t + kf_gain * r;
			pre_obj[i]->X = x.at<double>(0, 0);
			pre_obj[i]->Y = x.at<double>(1, 0);
			pre_obj[i]->Vx = x.at<double>(2, 0);
			pre_obj[i]->Vy = x.at<double>(3, 0);

		}
	}

	P = P - kf_gain*H*P;
	//delete[]temp;
	last_obj.empty();
	last_obj = pre_obj;
	cur_obj.empty();
	//}
}

#endif