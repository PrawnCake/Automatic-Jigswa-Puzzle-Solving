#include "utility_CornerIdentificaion.h"

using namespace std;
using namespace cv;

double utility_CornerIdentificaion::findAngleBetween(Point pt1, Point pt2)
{
	double angle=0.0;
	if (pt1.x - pt2.x != 0.0)
		angle = atan((double)(pt1.y - pt2.y) / (double)(pt1.x - pt2.x));
	else
		angle = CV_PI / 2.0; //90 degrees
	return angle;
}

double utility_CornerIdentificaion::findGradient(Point pt1, Point pt2)
{
	double gradient = 0.0;
	if (pt1.x - pt2.x != 0.0)
		gradient = atan2(pt2.y - pt1.y, pt2.x - pt1.x);
	else
	{
		gradient = CV_PI / 2.0; //90 degrees
		if (pt1.y > pt2.y)
			gradient = -gradient;
	}
	return gradient;
		
}

double utility_CornerIdentificaion::euclideanDist(Point& p, Point& q)
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
		
		gradient1Theta = findAngleBetween(pt1, pt2);
		
		
		gradient2Theta = findAngleBetween(pt2, pt3);

		angle = fabs(gradient2Theta - gradient1Theta) * 180.0 / CV_PI;

		if (angle >= 90 - angleThreshold && angle <= 90 + angleThreshold)
		{
			potentialCorners.push_back(pt2);
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

		if (!(euclideanDist(cnr1, cnr2) > approxWidth*0.7 && euclideanDist(cnr1, cnr2) < approxWidth*1.3)) //check if distance is within a threshold
		{
			cnr2Index = (cnr2Index + 1) % corners.size();
			continue;
		}

		if (euclideanDist(cnr1, centroid) / euclideanDist(cnr2, centroid) > 1.3 || euclideanDist(cnr2, centroid) / euclideanDist(cnr1, centroid) > 1.3) //check the ratio between the corners and their distances to the centroid
		{
			cnr2Index = (cnr2Index + 1) % corners.size();
			continue;
		}

		//check gradient between lines is +- 90 degrees
		double gradient1Theta, gradient2Theta;
		
		gradient1Theta = findAngleBetween(cnr1,centroid);
		gradient2Theta = findAngleBetween(cnr2, centroid);
		
		gradient1Theta = gradient1Theta * 180.0 / CV_PI;
		gradient2Theta = gradient2Theta * 180.0 / CV_PI;

		if (!(abs(gradient1Theta - gradient2Theta) < 120 && abs(gradient1Theta - gradient2Theta) > 60))
		{
			cnr2Index = (cnr2Index + 1) % corners.size();
			continue;
		}
		
		candidates.push_back(cnr2Index); //cnr2 meets all requirements to be a corner, push it to list
		cnr2Index = (cnr2Index + 1) % corners.size();
	}
	return candidates;
}

list<Point> utility_CornerIdentificaion::identifyTrueCorners(vector<Point> corners, Point centroid, int approxWidth)
{
	list<Point> trueCorners;
	int cnr1Index = 0, cnr2Index = 0, cnr3Index = 0, cnr4Index = 0;
	list<int> cnr2CandidatesIndex, cnr3CandidatesIndex, cnr4CandidatesIndex;
	
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
				double dist = utility_CornerIdentificaion::euclideanDist(corners[cnr3Index], corners[cnr2Index]);
				
				//find potential corner 4 candidates
				cnr4CandidatesIndex = findNextPotentialTrueCorners(corners, centroid, cnr3Index, approxWidth);
				while (!cnr4CandidatesIndex.empty())
				{

					cnr4Index = cnr4CandidatesIndex.front();
					cnr4CandidatesIndex.pop_front();

					double dist = utility_CornerIdentificaion::euclideanDist(corners[cnr1Index], corners[cnr4Index]);
					if (!(approxWidth*0.8 < dist && dist < approxWidth*1.2))
						continue;

					double cnr2cnr1 = utility_CornerIdentificaion::euclideanDist(corners[cnr2Index], corners[cnr4Index]);
					double cnr2cnr2 = utility_CornerIdentificaion::euclideanDist(corners[cnr1Index], corners[cnr3Index]);

					if (!(cnr2cnr2 / cnr2cnr1 > 0.8 && cnr2cnr2 / cnr2cnr1 < 1.2))
						continue;

					bool cnr1check = corners[cnr1Index] == corners[cnr2Index] || corners[cnr1Index] == corners[cnr3Index] || corners[cnr1Index] == corners[cnr4Index];
					bool cnr2check = corners[cnr2Index] == corners[cnr3Index] || corners[cnr2Index] == corners[cnr4Index];
					bool cnr3check = corners[cnr3Index] == corners[cnr4Index];
					if (cnr1check || cnr2check || cnr3check)
						continue;

					trueCorners.push_back(corners[cnr1Index]);
					trueCorners.push_back(corners[cnr2Index]);
					trueCorners.push_back(corners[cnr3Index]);
					trueCorners.push_back(corners[cnr4Index]);
					return trueCorners;
				}
			}
		}
	}
	return trueCorners;
}
