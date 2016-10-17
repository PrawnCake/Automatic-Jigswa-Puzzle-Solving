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
	//Functions
	Edge();
	~Edge();
	Edge(vector<Point> e, vector<Point> eActual, Mat img);	//Constructor
	
	void calcChangeInAngles();		//Records the change in angle between approximation points
	void calcLengthBetweenPoints();	//Records the distances between approximation points
	void determineEdgeType();		//Identifies the type of edge
	void computeEdgeStrip(Mat img);	//Generates an image strip along boundary

	//Attributes
	vector<double> changeInAngles;		//Change in angle between approximation points an edge
	vector<double> lengthBetweenPoints;	//Distances between approximation points an edge
	vector<Point> edgePoints;			//Vector array of each approximation point
	vector<Point> actualEdgePoints;		//Vector array of every point along the edge boundary
	EdgeType edgetype;					//Type of edge (straight, convex, concave)
	Mat edgeStrip;						//Sampled image strip from the computeEdgeStrip function
	
};

