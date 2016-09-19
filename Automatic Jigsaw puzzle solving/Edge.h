#pragma once
#include "utility_CornerIdentificaion.h"
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

enum EdgeType { STRAIGHT = 0, CONVEX = 1, CONCAVE = 2 };

class Edge
{
public:
	Edge(vector<Point> e);
	Edge();
	void calcChangeInAngles();
	void calcLengthBetweenPoints();
	vector<Point> getContour();
	void determineEdgeType();
	vector<double> changeInAngles;
	vector<double> lengthBetweenPoints;
	EdgeType getEdgeType();
	~Edge();
private:
	EdgeType edgetype;
	vector<Point> edgePoints;
	Mat edgeStrip;
};

