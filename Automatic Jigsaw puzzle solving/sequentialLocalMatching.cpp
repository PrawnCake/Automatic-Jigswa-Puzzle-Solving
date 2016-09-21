#include "sequentialLocalMatching.h"


double sequentialLocalMatching::localMatchShape(Edge e1, Edge e2)
{
	Mat test(1000, 1000, CV_8UC3, Scalar(0, 0, 0));

	if (e1.getEdgeType() == e2.getEdgeType() ) //edge types are the same, dont match
		return 10000.0;
	int pixelDiff = e1.getContour().size() - e2.getContour().size();
	//cout << "Pixel count difference: " << e1.getContour().size() << " " << e2.getContour().size() << " " << pixelDiff << "\n";
	
	if (e1.getEdgeType() == STRAIGHT || e2.getEdgeType() == STRAIGHT )
		return 20000.0;

	double areaDifference=0;
	
	Point e1p1(0, 0);
	Point e1p2(e1p1.x + e1.lengthBetweenPoints[0], e1p1.y);

	Point e2p1(0, 0);
	Point e2p2(e2p1.x + e2.lengthBetweenPoints[e2.lengthBetweenPoints.size()-1], e2p1.y);
	
	int index1 = 0;
	int index2 = e2.changeInAngles.size()-1;
	double angle1 = 0;
	double angle2 = 0;
	while (index1 < e1.changeInAngles.size() && index2 >= 0)
	{
		double halfPerim1 = ((double)(utility_CornerIdentificaion::euclideanDist(e1p1, e1p2) + utility_CornerIdentificaion::euclideanDist(e1p1, e2p1) + utility_CornerIdentificaion::euclideanDist(e1p2, e2p1))) /2.0;
		double halfPerim2 = ((double)(utility_CornerIdentificaion::euclideanDist(e2p1, e2p2) + utility_CornerIdentificaion::euclideanDist(e2p1, e1p2) + utility_CornerIdentificaion::euclideanDist(e2p2, e1p2))) /2.0;

		double area1 = sqrt(halfPerim1 * (halfPerim1 - utility_CornerIdentificaion::euclideanDist(e1p1, e1p2)) * (halfPerim1 - utility_CornerIdentificaion::euclideanDist(e1p1, e2p1)) * (halfPerim1 - utility_CornerIdentificaion::euclideanDist(e1p2, e2p1)));
		double area2 = sqrt(halfPerim2 * (halfPerim2 - utility_CornerIdentificaion::euclideanDist(e2p1, e2p2)) * (halfPerim2 - utility_CornerIdentificaion::euclideanDist(e2p1, e1p2)) * (halfPerim2 - utility_CornerIdentificaion::euclideanDist(e2p2, e1p2)));

		areaDifference += (area1 + area2);

		//line(test, Point(e1p1.x + 500, e1p1.y + 500), Point(e1p2.x + 500, e1p2.y + 500),Scalar(255,0,0),2);
		//line(test, Point(e2p1.x + 500, e2p1.y + 500), Point(e2p2.x + 500, e2p2.y + 500), Scalar(0, 255, 0), 2);

		e1p1 = Point(e1p2.x,e1p2.y);
		e2p1 = Point(e2p2.x,e2p2.y);

		angle1 += e1.changeInAngles[index1];
		angle2 += -e2.changeInAngles[index2];

		int e1NewX = e1p2.x + e1.lengthBetweenPoints[index1 + 1] * cos(angle1 * CV_PI / 180.0);
		int e1NewY = e1p2.y + e1.lengthBetweenPoints[index1 + 1] * sin(angle1 * CV_PI / 180.0);

		int e2NewX = e2p2.x + e2.lengthBetweenPoints[index2] * cos(angle2 * CV_PI / 180.0);
		int e2NewY = e2p2.y + e2.lengthBetweenPoints[index2] * sin(angle2 * CV_PI / 180.0);

		e1p2 = Point(e1NewX, e1NewY);
		e2p2 = Point(e2NewX, e2NewY);

		index1++;
		index2--;

	}
	//line(test, Point(e1p1.x + 500, e1p1.y + 500), Point(e1p2.x + 500, e1p2.y + 500), Scalar(255, 0, 0), 2);
	//line(test, Point(e2p1.x + 500, e2p1.y + 500), Point(e2p2.x + 500, e2p2.y + 500), Scalar(0, 255, 0), 2);
	imshow("meh", test);
	return areaDifference;
	
}

double sequentialLocalMatching::localMatchImage(Edge e1, Edge e2, Mat img)
{
	Mat test(500, 500, CV_8UC3, Scalar(255, 0, 0));
	namedWindow("sup", WINDOW_NORMAL);
	int yIncrement = 0;
	
	for (int m = 0; m < e1.getActualContour().size() - 1; m++) //
	{
		Point pt1 = e1.getActualContour()[m];
		Point pt2 = e1.getActualContour()[m + 1];
		double gradientTheta;

		if (pt1.x - pt2.x != 0.0)
			gradientTheta = atan2(pt2.y - pt1.y, pt2.x - pt1.x);
		else
			gradientTheta = CV_PI / 2.0; //90 degrees

		float uCol = (float)sin(-gradientTheta);
		float vCol = (float)cos(-gradientTheta);
		float uRow = vCol;
		float vRow = -uCol;

		float startu = pt1.x;
		float startv = pt1.y;

		for (int i = 0; i < 10; i++)
		{
			float u = startu - uCol;
			float v = startv - vCol;

			 
			for (int j = 0; j < (int)utility_CornerIdentificaion::euclideanDist(pt1, pt2); j++)
			{
				test.at<Vec3b>(Point(i, j + yIncrement)) = img.at<Vec3b>(Point(u, v));
				//img.at<Vec3b>(Point(u, v)) = Vec3b(0, 255, 255);
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
	
	//**************************************PIECE 2 CLEAN THIS UP***************************************
	yIncrement = 0;
	int xIncrement = 10;
	for (int m = 0; m < e2.getActualContour().size() - 1; m++)
	{
		Point pt1 = e2.getActualContour()[e2.getActualContour().size() - 1 - m];
		Point pt2 = e2.getActualContour()[e2.getActualContour().size() - 2 - m];
		double gradientTheta;

		if (pt1.x - pt2.x != 0.0)
			gradientTheta = atan2(pt2.y - pt1.y, pt2.x - pt1.x);
		else
			gradientTheta = CV_PI / 2.0; //90 degrees

		float uCol = (float)sin(-gradientTheta);
		float vCol = (float)cos(-gradientTheta);
		float uRow = vCol;
		float vRow = -uCol;

		float startu = pt1.x;
		float startv = pt1.y;

		for (int i = 0; i < 10; i++)
		{
			float u = startu +uCol;
			float v = startv +vCol;


			for (int j = 0; j < (int)utility_CornerIdentificaion::euclideanDist(pt1, pt2); j++)
			{
				test.at<Vec3b>(Point(i + xIncrement, j + yIncrement)) = img.at<Vec3b>(Point(u, v));
				//img.at<Vec3b>(Point(u, v)) = Vec3b(0, 255, 255);
				//increment u and v by the row vectors
				u += uRow;
				v += vRow;

			}
			//increment rowu and rowv by column vectors
			startu += uCol;
			startv += vCol;

		}
		yIncrement += (int)utility_CornerIdentificaion::euclideanDist(pt1, pt2);
	}
	//blur(test, test,Size(11,1));
	imshow("sup", test);
	return 0.0;
}