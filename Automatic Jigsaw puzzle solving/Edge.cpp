#include "Edge.h"

using namespace std;
using namespace cv;


Edge::Edge()
{

}

Edge::Edge(vector<Point> e, vector<Point> eActual, Mat img)
{
	actualEdgePoints = eActual;
	edgePoints = e;
	calcChangeInAngles();
	calcLengthBetweenPoints();
	computeEdgeStrip(img);
}



void Edge::computeEdgeStrip(Mat img)
{
	Mat imgCopy;
	img.copyTo(imgCopy);
	int pixelDepth = 3;
	int yIncrement = 0;
	vector<Point> contour = actualEdgePoints;
	Mat edgeImage(contour.size(), pixelDepth, CV_8UC3, Scalar(255, 255, 255));
	for (int m = 0; m < contour.size() - 1; m++)
	{
		Point pt1 = contour[m];
		Point pt2 = contour[m + 1];
		double gradientTheta;
		gradientTheta = utility_CornerIdentificaion::findGradient(pt1, pt2);

		float uCol = (float)sin(-gradientTheta);
		float vCol = (float)cos(-gradientTheta);
		float uRow = vCol;
		float vRow = -uCol;

		float startu = pt1.x;
		float startv = pt1.y;

		for (int i = 0; i < pixelDepth; i++)
		{
			float u = startu - uCol;
			float v = startv - vCol;


			for (int j = 0; j < (int)utility_CornerIdentificaion::euclideanDist(pt1, pt2); j++)
			{
				edgeImage.at<Vec3b>(Point(i, j + yIncrement)) = imgCopy.at<Vec3b>(Point(u, v));
				//imgCopy.at<Vec3b>(Point(u, v)) = Vec3b(0, 255, 255);
				//increment u and v by the row vectors
				u += uRow;
				v += vRow;

			}
			//increment rowu and rowv by column vectors
			startu -= uCol;
			startv -= vCol;

		}
		yIncrement += (int)utility_CornerIdentificaion::euclideanDist(pt1, pt2);
	}
	namedWindow("area scanned", WINDOW_NORMAL);
	imshow("area scanned",imgCopy);

	int r = 0;
	int g = 0;
	int b = 0;

	for (int i = 0; i < edgeImage.rows; i++)
	{
		for (int j = 0; j < pixelDepth; j++)
		{
			b += edgeImage.at<Vec3b>(Point(j, i))[0];
			g += edgeImage.at<Vec3b>(Point(j, i))[1];
			r += edgeImage.at<Vec3b>(Point(j, i))[2];
		}
		b = b / pixelDepth;
		g = g / pixelDepth;
		r = r / pixelDepth;

		for (int j = 0; j < pixelDepth; j++)
		{
			edgeImage.at<Vec3b>(Point(j, i))[0] = b;
			edgeImage.at<Vec3b>(Point(j, i))[1] = g;
			edgeImage.at<Vec3b>(Point(j, i))[2] = r;
		}
		b = 0;
		g = 0;
		r = 0;
	}
	edgeImage.copyTo(edgeStrip);
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
	double threshold = 2;


		gradientBetweenCorners = utility_CornerIdentificaion::findGradient(cnr1, cnr2);//atan((double)(cnr1.y - cnr2.y) / (double)(cnr1.x - cnr2.x));
		gradientCnr1ToPoint = utility_CornerIdentificaion::findGradient(cnr1, pointOfMaxDistance);
		


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


Edge::~Edge()
{
}
