#include "Edge.h"

using namespace std;
using namespace cv;


Edge::Edge()
{

}

Edge::Edge(vector<Point> e, vector<Point> eActual )
{
	actualEdgePoints = eActual;
	edgePoints = e;
	calcChangeInAngles();
	calcLengthBetweenPoints();
}


void Edge::calcChangeInAngles()
{
	list<double> angles;

	if (edgePoints.size() <= 2)
	{
		changeInAngles = { 0 };
		return;
	}
		


	for (int i = 0; i < edgePoints.size()-1; i++)
	{
		Point pt1 = edgePoints[i];
		Point pt2 = edgePoints[i+1];
		double gradientTheta;

		if (pt1.x - pt2.x != 0.0)
			gradientTheta = atan2(pt2.y - pt1.y, pt2.x - pt1.x);
		else
			gradientTheta = CV_PI / 2.0; //90 degrees

		gradientTheta = gradientTheta * 180.0 / CV_PI;
		//if (gradientTheta < 0)
			gradientTheta = 180 + gradientTheta;
		angles.push_back(gradientTheta);
	}
	
	list<double> changeInAnglesL;
	double angle1;
	double angle2;
	
	
	angle1 = angles.front();
	angles.pop_front();
	angle2 = angles.front();
	angles.pop_front();
	
	while (!angles.empty())
	{
		
		changeInAnglesL.push_back((angle2 - angle1));

		angle1 = angle2;
		angle2 = angles.front();
		angles.pop_front();
	}
	changeInAnglesL.push_back((angle2 - angle1));
	changeInAngles = { begin(changeInAnglesL), end(changeInAnglesL) };
}

void Edge::calcLengthBetweenPoints()
{
	list<double> distancesL;
	for (int i = 0; i < edgePoints.size() - 1; i++)
	{
		distancesL.push_back((double)utility_CornerIdentificaion::euclideanDist(edgePoints[i], edgePoints[i+1]));
	}
	lengthBetweenPoints = { begin(distancesL), end(distancesL) };
}

void Edge::determineEdgeType()
{
	double maxDistance = 0;
	int maxPointIndex = 1;
	double distance;
	char side = 'C';
	Point line_start = edgePoints[0];
	Point line_end = edgePoints[edgePoints.size() - 1];
	for (int i = 1; i < edgePoints.size()-1; i++)
	{
		Point point = edgePoints[i];
		double normalLength = _hypot(line_end.x - line_start.x, line_end.y - line_start.y);
		double distance = (double)((point.x - line_start.x) * (line_end.y - line_start.y) - (point.y - line_start.y) * (line_end.x - line_start.x)) / normalLength;

		if (fabs(distance) > maxDistance)
		{
			maxDistance = fabs(distance);
			maxPointIndex = i;
			if (distance < 0)
				side = 'V';
			else
				side = 'C';
		}
	}
	Point cnr1 = edgePoints[0];
	Point cnr2 = edgePoints[edgePoints.size()-1];
	Point pointOfMaxDistance = edgePoints[maxPointIndex];
	double gradientBetweenCorners;
	double gradientCnr1ToPoint;
	double threshold = 5;

	if (cnr1.x - cnr2.x != 0.0)
		gradientBetweenCorners = atan((double)(cnr1.y - cnr2.y) / (double)(cnr1.x - cnr2.x));
	else
		gradientBetweenCorners = CV_PI / 2.0; //90 degrees

	if (cnr1.x - pointOfMaxDistance.x != 0.0)
		gradientCnr1ToPoint = atan((double)(cnr1.y - pointOfMaxDistance.y) / (double)(cnr1.x - pointOfMaxDistance.x));
	else
		gradientCnr1ToPoint = CV_PI / 2.0; //90 degrees


	double angleDiff = fabs(gradientBetweenCorners - gradientCnr1ToPoint) * 180.0 / CV_PI;

	
	if (angleDiff > threshold && side == 'C' && maxDistance > 10)  //edge is convex
	{
		edgetype = CONCAVE;
	}
	else if (angleDiff > threshold && side == 'V' && maxDistance > 10)  //edge is concave
	{
		edgetype = CONVEX;
	}
	else
		edgetype = STRAIGHT;
}

EdgeType Edge::getEdgeType()
{
	return edgetype;
}


vector<Point> Edge::getContour()
{
	return edgePoints;
}

vector<Point>Edge::getActualContour()
{
	return actualEdgePoints;
}
Edge::~Edge()
{
}
