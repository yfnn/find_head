#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "object.h"
#include "getHeightmap.h"
using namespace cv;
using namespace std;

void paramsCalculate(Mat heightmap_d, Mat heightmap_head, Obj& object)
{
	int x, y;
	int w, h;
	float height;

	namedWindow("hm_d");
	imshow("hm_d", heightmap_d);
	namedWindow("hm_head");
	imshow("hm_head", heightmap_head);
	waitKey(1);

	Mat height_t1;
	heightmap_head.copyTo(height_t1);

	vector<vector<Point> > contours_head;
	vector<Vec4i> hierarchy_head;
	findContours(height_t1, contours_head, hierarchy_head, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	int num_head = 0;
	int num_fg = 0;
	vector<Point > v_point;
	vector<Rect > v_rect;
	vector<Rect > v_rect_head;
	for (int i = 0; i < contours_head.size(); i++)
	{
		double tempArea = contourArea(contours_head[i]);
		if (tempArea > 10)
		{
			num_head ++;
			Point p;
			Moments mom = moments(Mat(contours_head[i]));
			x = mom.m10 / mom.m00;
			y = mom.m01 / mom.m00;
			/*x = (x - 127) * 3;
			y = (y - 127) * 3;*/
			p.x = x;
			p.y = y;
			v_point.push_back(p);
			Rect rect = boundingRect(contours_head[i]);
			/*rect.x = (rect.x - 127) * 3;
			rect.y = (rect.y - 127) * 3;
			rect.width = rect.width * 3;
			rect.height = rect.height * 3;*/
			v_rect_head.push_back(rect);
		}
	}
	Mat height_t2;
	heightmap_d.copyTo(height_t2);
	vector<vector<Point> > contours_fg;
	vector<Vec4i> hierarchy_fg;
	findContours(height_t2, contours_fg, hierarchy_fg, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	for (int i = 0; i < contours_fg.size(); i++)
	{
		double tempArea = contourArea(contours_fg[i]);
		if (tempArea > 30)
		{
			num_fg++;
			Rect rect = boundingRect(contours_fg[i]);
			/*rect.x = (rect.x - 127) * 3;
			rect.y = (rect.y - 127) * 3;
			rect.width = rect.width * 3;
			rect.height = rect.height * 3;*/
			v_rect.push_back(rect);
		}
	}
	if (num_head == num_fg){
		double max_h;
		Point maxIdx;
		for (int i = 0; i < v_point.size(); i++){
			for (int j = 0; j < v_rect.size(); j++){
				if (v_point[i].x > v_rect[j].x && v_point[i].x < (v_rect[j].x + v_rect[j].width) && v_point[i].y > v_rect[j].y && v_point[i].y < (v_rect[j].y + v_rect[j].height)){
					
					Mat p = heightmap_head(v_rect[j]);
					minMaxLoc(p, NULL, &max_h, NULL, &maxIdx);
					height = max_h;
					object.push((v_point[i].x-127)*3, (v_point[i].y-127)*3, (v_rect[j].y-127)*3, (v_rect[j].y + v_rect[j].height-127)*3, (v_rect[j].x-127)*3, (v_rect[j].x + v_rect[j].width-127)*3, height);
					break;
				}
			}
		}
	}
	if (num_head != num_fg){
		double max_h;
		Point maxIdx;
		int n = 0;
		vector<Point > temp;
		vector<Rect > temp_head;
		for (int i = 0; i < v_rect.size(); i++){
			int j = 0;
			for (; j < v_point.size(); j++){
				if (v_point[j].x > v_rect[i].x && v_point[j].x < (v_rect[i].x + v_rect[i].width) && v_point[j].y > v_rect[i].y && v_point[j].y < (v_rect[i].y + v_rect[i].height)){
					temp.push_back(v_point[j]);
					temp_head.push_back(v_rect_head[j]);
					n++;
				}
			}
			if (n == 1){
				Mat p = heightmap_head(v_rect[i]);
				minMaxLoc(p, NULL, &max_h, NULL, &maxIdx);
				height = max_h;
				object.push((temp[0].x-127)*3, (temp[0].y-127)*3, (v_rect[i].y-127)*3, (v_rect[i].y + v_rect[i].height-127)*3, (v_rect[i].x-127)*3, (v_rect[i].x + v_rect[i].width-127)*3, height);
			}
			if (n == 0){
				Mat p = heightmap_head(v_rect[i]);
				minMaxLoc(p, NULL, &max_h, NULL, &maxIdx);
				height = max_h;
				object.push((v_rect[i].x+v_rect[i].width/2-127)*3, (v_rect[i].y+v_rect[i].height/2-127)*3, (v_rect[i].y-127)*3, (v_rect[i].y + v_rect[i].height-127)*3, (v_rect[i].x-127)*3, (v_rect[i].x + v_rect[i].width-127)*3, height);
			}
			if (n == 2){//2人,区域分成两部分，每人占2/3
				if (v_rect[i].height > v_rect[i].width){
					for (int m = 0; m < 2; m++){
						Mat p = heightmap_head(temp_head[m]);
						minMaxLoc(p, NULL, &max_h, NULL, &maxIdx);
						height = max_h;
						object.push((temp[m].x-127)*3, (temp[m].y-127)*3, abs(v_rect[i].y - temp[m].y)<v_rect[i].height / 2 ? (v_rect[i].y-127)*3 : (v_rect[i].y + v_rect[i].height / 3-127)*3, abs(v_rect[i].y - temp[m].y)<v_rect[i].height / 2 ? (v_rect[i].y+v_rect[i].height*2/3-127)*3 : (v_rect[i].y + v_rect[i].height-127)*3, (v_rect[i].x-127)*3, (v_rect[i].x + v_rect[i].width-127)*3, height);
					}
				}
				else{
					for (int m = 0; m < 2; m++){
						Mat p = heightmap_head(temp_head[m]);
						minMaxLoc(p, NULL, &max_h, NULL, &maxIdx);
						height = max_h;
						object.push((temp[m].x-127)*3, (temp[m].y-127)*3, (v_rect[i].y-127)*3, (v_rect[i].y + v_rect[i].height-127)*3, abs(v_rect[i].x - temp[m].x)<v_rect[i].width / 2 ? (v_rect[i].x-127)*3 : (v_rect[i].x + v_rect[i].width / 3-127)*3, abs(v_rect[i].x - temp[m].x)<v_rect[i].width / 2 ? (v_rect[i].x+v_rect[i].width*2/3-127)*3 : (v_rect[i].x + v_rect[i].width-127)*3, height);
					}
				}
			}
			if (n > 2){
				for (int m = 0; m < n; m++){
					Mat p = heightmap_head(temp_head[m]);
					minMaxLoc(p, NULL, &max_h, NULL, &maxIdx);
					height = max_h;
					object.push((temp[m].x-127)*3, (temp[m].y-127)*3, (temp[m].y-127)*3 - 25, (temp[m].y-127)*3 + 25, (temp[m].x-127)*3 - 25, (temp[m].x-127)*3 + 25, height);
				}
			}
			n = 0;
		}
	}
}