#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

void find_head(int th_height1, int th_height2, int th_height3, double th_Area1, double th_Area2, Mat origin_img, Mat& getHead)
{
	/*Mat close_img;
	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
	
	morphologyEx(origin_img, close_img, CV_MOP_CLOSE, element);*/
	
	
	Mat head;
	
	threshold(origin_img, head, th_height1, 255, THRESH_TOZERO);
	namedWindow("origin_img");
	imshow("origin_img", origin_img);

	namedWindow("origin_H");
	imshow("origin_H", head);
	waitKey(1);

	
	Mat head_marker(origin_img.rows, origin_img.cols, CV_32S, Scalar::all(0));
	Mat head_t;
	head.copyTo(head_t);
	vector<vector<Point> > head_contours;
	vector<Vec4i> head_hierarchy;
	
	findContours(head_t, head_contours, head_hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	/*pointPolygonTest();*/
	
	Mat temp(origin_img.rows, origin_img.cols, CV_8U, Scalar::all(0));
	Mat marker(origin_img.rows, origin_img.cols, CV_32S, Scalar::all(0));
	uchar* p_temp = temp.data;
	int* p_marker = (int*)marker.data;
	uchar* p_head = head.data;
	for (int i = 0; i < head_contours.size(); i++)
	{
		double tempArea = contourArea(head_contours[i]);
		if (tempArea >= th_Area1)
		{
			drawContours(marker, head_contours, i, Scalar::all(1), -1, 8, head_hierarchy, INT_MAX);
			for (int i1 = 0; i1 < 177*244; i1++)
			{
				if (p_marker[i1] != 0)
					p_temp[i1] = p_head[i1];
				else
					p_temp[i1] = 0;
			}
			memset(p_marker,0,177*244*sizeof(int));
			double max_height;
			minMaxLoc(temp, NULL, &max_height);
			Mat head_narrow(origin_img.rows, origin_img.cols, CV_8U, Scalar::all(0));
			if (max_height > th_height3)
			{
				threshold(temp, head_narrow, th_height3, 255, THRESH_TOZERO);
				vector<vector<Point> > contours;
				vector<Vec4i> hierarchy;
				findContours(head_narrow, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
				int valid_subarea = 0;
				for (int j = 0; j < contours.size(); j++)
				{
					double temp_area = contourArea(contours[j]);
					if (temp_area >= th_Area2)
					{
						
						drawContours(head_marker, contours, j, Scalar::all(1), -1, 8, hierarchy, INT_MAX);
						valid_subarea++;
						
					}
				}
				if (valid_subarea == 0)
					drawContours(head_marker, head_contours, i, Scalar::all(1), -1, 8, head_hierarchy, INT_MAX);
				
			}
			else if (max_height > th_height2)
			{
				threshold(temp, head_narrow, th_height2, 255, THRESH_TOZERO);
				vector<vector<Point> > contours;
				vector<Vec4i> hierarchy;
				findContours(head_narrow, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
				int valid_subarea = 0;
				for (int j = 0; j < contours.size(); j++)
				{
					double temp_area = contourArea(contours[j]);
					if (temp_area >= th_Area2)
					{
						drawContours(head_marker, contours, j, Scalar::all(1), -1, 8, hierarchy, INT_MAX);
						valid_subarea++;
					}
				}
				if (valid_subarea == 0)
					drawContours(head_marker, head_contours, i, Scalar::all(1), -1, 8, head_hierarchy, INT_MAX);
			}
			else
			{
				
				drawContours(head_marker, head_contours, i, Scalar::all(1), -1, 8, head_hierarchy, INT_MAX);
				
			}
		}
	}
	int* p_head_marker = (int*)head_marker.data;
	uchar* p_gethead = getHead.data;
	for (int i2 = 0; i2<(origin_img.rows*origin_img.cols); i2++)
	{
		if (p_head_marker[i2] != 0)
			p_gethead[i2] = p_head[i2];
	}
	//head_marker.convertTo(head_marker, CV_8U);
	/*Mat getHead;*/
	//getHead = head.mul(head_marker);
}