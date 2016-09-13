#include "utility_CornerIdentificaion.h"

using namespace std;
using namespace cv;

float utility_CornerIdentificaion::euclideanDist(Point& p, Point& q)
{
	Point diff = p - q;
	return cv::sqrt(diff.x*diff.x + diff.y*diff.y);
}

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


list<int> utility_CornerIdentificaion::findNextPotentialTrueCorners(vector<Point> corners, Point centroid, int initialPointIndex, int approxWidth)
{
	Point cnr1, cnr2;
	list<int> candidates;
	cnr1 = corners[initialPointIndex];
	int cnr2Index = (initialPointIndex + 1) % corners.size();
	
	while (true)
	{

		cnr2 = corners[cnr2Index];
		if (initialPointIndex == cnr2Index)
		{
			return candidates;
		}

		if (!(euclideanDist(cnr1, cnr2) > approxWidth*0.8 && euclideanDist(cnr1, cnr2) < approxWidth*1.2)) //check if distance is within a threshold
		{
			cnr2Index = (cnr2Index + 1) % corners.size();
			continue;
		}

		if (euclideanDist(cnr1, centroid) / euclideanDist(cnr2, centroid) > 1.2 || euclideanDist(cnr2, centroid) / euclideanDist(cnr1, centroid) > 1.2) //check the ratio between the corners and their distances to the centroid
		{
			cnr2Index = (cnr2Index + 1) % corners.size();
			continue;
		}

		//check gradient between lines is +- 90 degrees
		double gradient1Theta, gradient2Theta;
		if (cnr1.x - centroid.x != 0.0)
			gradient1Theta = atan((double)(cnr1.y - centroid.y) / (double)(cnr1.x - centroid.x));
		else
			gradient1Theta = CV_PI / 2.0;

		if (cnr2.x - centroid.x != 0.0)
			gradient2Theta = atan((double)(cnr2.y - centroid.y) / (double)(cnr2.x - centroid.x));
		else
			gradient2Theta = CV_PI / 2.0;

		gradient1Theta = gradient1Theta * 180.0 / CV_PI;
		gradient2Theta = gradient2Theta * 180.0 / CV_PI;

		if (!(abs(gradient1Theta - gradient2Theta) < 110 && abs(gradient1Theta - gradient2Theta) > 70))
		{
			cnr2Index = (cnr2Index + 1) % corners.size();
			continue;
		}
		
		candidates.push_front(cnr2Index); //cnr2 meets all requirements to be a corner, push it to list
		cnr2Index = (cnr2Index + 1) % corners.size();
	}
	return candidates;
}

list<Point> utility_CornerIdentificaion::identifyTrueCorners(vector<Point> corners, Point centroid, int approxWidth)
{
	list<Point> trueCorners;
	int cnr1Index = 0, cnr2Index = 0, cnr3Index = 0, cnr4Index = 0;
	list<int> cnr2CandidatesIndex, cnr3CandidatesIndex, cnr4CandidatesIndex;
	int cornersFound = 1;
	
	//assume first point is the first corner
	

	for (cnr1Index = 0; cnr1Index < corners.size(); cnr1Index++)
	{
		//find potential corner 2 candidates
		cnr2CandidatesIndex = findNextPotentialTrueCorners(corners, centroid, cnr1Index, approxWidth);
		while (!cnr2CandidatesIndex.empty())
		{
			cnr2Index = cnr2CandidatesIndex.front();
			cnr2CandidatesIndex.pop_front();

			//find potential corner 3 candidates
			cnr3CandidatesIndex = findNextPotentialTrueCorners(corners, centroid, cnr2Index, approxWidth);
			while (!cnr3CandidatesIndex.empty())
			{
				cnr3Index = cnr3CandidatesIndex.front();
				cnr3CandidatesIndex.pop_front();

				//find potential corner 4 candidates
				cnr4CandidatesIndex = findNextPotentialTrueCorners(corners, centroid, cnr3Index, approxWidth);
				while (!cnr4CandidatesIndex.empty())
				{
					cnr4Index = cnr4CandidatesIndex.front();
					cnr4CandidatesIndex.pop_front();

					trueCorners.push_front(corners[cnr1Index]);
					trueCorners.push_front(corners[cnr2Index]);
					trueCorners.push_front(corners[cnr3Index]);
					trueCorners.push_front(corners[cnr4Index]);
					return trueCorners;
				}
			}
		}
	}
	return trueCorners;
}