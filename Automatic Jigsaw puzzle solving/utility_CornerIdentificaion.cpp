#include "utility_CornerIdentificaion.h"

using namespace std;
using namespace cv;

list<Point> utility_CornerIdentificaion::identifyCorners(vector<Point> contours)
{
	list<Point> potentialCorners;
	Point pt1;
	Point pt2;
	Point pt3;
	double gradient1Theta = 0;
	double gradient2Theta = 0;
	double angle;
	double angleThreshold = 20;

	for (int i = 0; i < contours.size(); i++)
	{
		//potentialCorners.push_front(contours[i]);
		gradient2Theta = gradient1Theta;

		pt1 = contours[i];
		if (i + 1 == contours.size())
			pt2 = contours[0];
		else
			pt2 = contours[i + 1];

		if (i + 2 == contours.size())
			pt3 = contours[0];
		else if (i + 2 > contours.size())
			pt3 = contours[1];
		else
			pt3 = contours[i + 2];



		if (pt1.x - pt2.x != 0.0)
			gradient1Theta = atan((pt1.y - pt2.y) / (pt1.x - pt2.x));
		else
			gradient1Theta = CV_PI / 2.0; //90 degrees



		if (pt2.x - pt3.x != 0.0)
			gradient2Theta = atan((pt2.y - pt3.y) / (pt2.x - pt3.x));
		else
			gradient2Theta = CV_PI / 2.0; //90 degrees



		angle = fabs(gradient2Theta - gradient1Theta) * 180.0 / CV_PI;

		if (angle >= 90 - angleThreshold && angle <= 90 + angleThreshold)
		{
			potentialCorners.push_front(pt2);
		}
	}
	return potentialCorners;
}
