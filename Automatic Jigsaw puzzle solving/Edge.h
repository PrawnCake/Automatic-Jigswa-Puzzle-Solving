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
	//functions
	Edge();
	~Edge();
	Edge(vector<Point> e, vector<Point> eActual, Mat img);	//Constructor, takes in the poly approximation and actual boundary of the edge 
	
	void calcChangeInAngles();			//Traverses the poly approximation of the edge and records the change in angle between approximation points
	void calcLengthBetweenPoints();		//Traverses the poly approximation of the edge and records the distances between approximation points
	void determineEdgeType();			//Identifies the type of edge
	void computeEdgeStrip(Mat img);		//Generates an image comprised of the average RGB value of several pixel deep at each point along the actual contour

	//attributes
	vector<double> changeInAngles;		//Vector array to store the change in between approximation points an edge
	vector<double> lengthBetweenPoints;	//Vector array to store the distances between approximation points an edge
	vector<Point> edgePoints;			//Vector array of each approximation point
	vector<Point> actualEdgePoints;		//Vector array of every point along the edge boundary
	EdgeType edgetype;					//Stores the type of edge (straight, convex, concave)
	Mat edgeStrip;						//Stores the sampled image strip from the computeEdgeStrip function
	
};

