#pragma once
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;
using namespace cv;


class utility_CornerIdentificaion
{
public:
	static list<Point> identifyCorners(vector<Point> contours);
	static list<Point> identifyTrueCorners(vector<Point> corners, Point centroid, int approxWidth);
private:
	static float euclideanDist(Point& p, Point& q);
	static list<int> findNextPotentialTrueCorners(vector<Point> corners, Point centroid, int initialPointIndex, int approxWidth);
};

