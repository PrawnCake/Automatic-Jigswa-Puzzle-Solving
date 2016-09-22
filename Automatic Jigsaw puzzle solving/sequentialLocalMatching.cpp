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

		line(test, Point(e1p1.x + 500, e1p1.y + 500), Point(e1p2.x + 500, e1p2.y + 500),Scalar(255,0,0),2);
		line(test, Point(e2p1.x + 500, e2p1.y + 500), Point(e2p2.x + 500, e2p2.y + 500), Scalar(0, 255, 0), 2);

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
	line(test, Point(e1p1.x + 500, e1p1.y + 500), Point(e1p2.x + 500, e1p2.y + 500), Scalar(255, 0, 0), 2);
	line(test, Point(e2p1.x + 500, e2p1.y + 500), Point(e2p2.x + 500, e2p2.y + 500), Scalar(0, 255, 0), 2);
	imshow("meh", test);
	return areaDifference;
	
}

double sequentialLocalMatching::localMatchImage(Edge e1, Edge e2, Mat img)
{
	Mat imgCopy;
	img.copyTo(imgCopy);
	vector<Point> contour1 = e1.getActualContour();
	vector<Point> contour2 = e2.getActualContour();
	int pixelDepth = 3;
	int longestApprox;
	if (contour1.size() > contour2.size())
		longestApprox = contour1.size();
	else
		longestApprox = contour2.size();
	Mat test(longestApprox, pixelDepth * 2, CV_8UC3, Scalar(255, 255, 255));
	namedWindow("sup", WINDOW_NORMAL);
	namedWindow("asdfgh", WINDOW_NORMAL);
	int yIncrement = 0;
	
	for (int m = 0; m < contour1.size() - 1; m++) //
	{
		Point pt1 = contour1[m];
		Point pt2 = contour1[m + 1];
		double gradientTheta;
		gradientTheta = utility_CornerIdentificaion::findGradient(pt1,pt2);	

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
				test.at<Vec3b>(Point(i, j + yIncrement)) = imgCopy.at<Vec3b>(Point(u, v));
				imgCopy.at<Vec3b>(Point(u, v)) = Vec3b(0, 255, 255);
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
	int xIncrement = pixelDepth;

	for (int m = 0; m < contour2.size() - 1; m++)
	{
		Point pt1 = contour2[contour2.size() - 1 - m];
		Point pt2 = contour2[contour2.size() - 2 - m];
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
			float u = startu +uCol;
			float v = startv +vCol;


			for (int j = 0; j < (int)utility_CornerIdentificaion::euclideanDist(pt1, pt2); j++)
			{
				test.at<Vec3b>(Point(i + xIncrement, j + yIncrement)) = imgCopy.at<Vec3b>(Point(u, v));
				imgCopy.at<Vec3b>(Point(u, v)) = Vec3b(0, 255, 255);
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
	///Average out pixels
	int r = 0;
	int g = 0;
	int b = 0;

	for (int i = 0; i < test.rows; i++)
	{
		for (int j = 0; j < pixelDepth; j++)
		{
			b += test.at<Vec3b>(Point(j, i))[0];
			g += test.at<Vec3b>(Point(j, i))[1];
			r += test.at<Vec3b>(Point(j, i))[2];
		}
		b = b / pixelDepth;
		g = g / pixelDepth;
		r = r / pixelDepth;

		for (int j = 0; j < pixelDepth; j++)
		{
			test.at<Vec3b>(Point(j, i))[0] = b;
			test.at<Vec3b>(Point(j, i))[1] = g;
			test.at<Vec3b>(Point(j, i))[2] = r;
		}
		b = 0;
		g = 0;
		r = 0;
	}

	r = 0;
	g = 0;
	b = 0;

	//2nd half
	for (int i = 0; i < test.rows; i++)
	{
		for (int j = pixelDepth; j < pixelDepth*2; j++)
		{
			b += test.at<Vec3b>(Point(j, i))[0];
			g += test.at<Vec3b>(Point(j, i))[1];
			r += test.at<Vec3b>(Point(j, i))[2];
		}
		b = b / pixelDepth;
		g = g / pixelDepth;
		r = r / pixelDepth;

		for (int j = pixelDepth; j < pixelDepth * 2; j++)
		{
			test.at<Vec3b>(Point(j, i))[0] = b;
			test.at<Vec3b>(Point(j, i))[1] = g;
			test.at<Vec3b>(Point(j, i))[2] = r;
		}
		b = 0;
		g = 0;
		r = 0;
	}
	cvtColor(test, test, CV_BGR2GRAY);
	imshow("asdfgh", test);
	int paletIndex = 4;
	int* paletBin = new int[paletIndex];
	for (int i = 0; i < paletIndex; i++)
	{
		paletBin[i] = 0;
	}
	
	for (int i = 0; i < test.cols; i++)
	{

		for (int j = 0; j < test.rows; j++)
		{
			int value = test.at<uchar>(Point(i, j));

			for (int k = 0; k < paletIndex; k++)
			{
				if (value < (k + 1) * (256 / paletIndex))
				{
					test.at<uchar>(Point(i, j)) = k*(256 / paletIndex);
					paletBin[k]++;
					break;
				}
			}
		}
	}

	//Score match
	int mostOccuringBinIndex = 0;
	for (int i = 0; i < paletIndex; i++)
	{
		if (paletBin[i] > paletBin[mostOccuringBinIndex])
			mostOccuringBinIndex = i;
		cout << paletBin[i] << endl;
	}
	imshow("sup", test);
	imshow("ishdf",imgCopy);
	return 0.0;
}