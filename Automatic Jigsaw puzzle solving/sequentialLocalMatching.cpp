#include "sequentialLocalMatching.h"


double sequentialLocalMatching::localMatchShape(Edge e1, Edge e2)
{
	Mat edgeSideBySide(1000, 1000, CV_8UC3, Scalar(0, 0, 0));

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

		line(edgeSideBySide, Point(e1p1.x + 500, e1p1.y + 500), Point(e1p2.x + 500, e1p2.y + 500),Scalar(255,0,0),2);
		line(edgeSideBySide, Point(e2p1.x + 500, e2p1.y + 500), Point(e2p2.x + 500, e2p2.y + 500), Scalar(0, 255, 0), 2);

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
	line(edgeSideBySide, Point(e1p1.x + 500, e1p1.y + 500), Point(e1p2.x + 500, e1p2.y + 500), Scalar(255, 0, 0), 2);
	line(edgeSideBySide, Point(e2p1.x + 500, e2p1.y + 500), Point(e2p2.x + 500, e2p2.y + 500), Scalar(0, 255, 0), 2);
	imshow("meh", edgeSideBySide);
	return areaDifference;
	
}

double sequentialLocalMatching::localMatchImage(Edge e1, Edge e2)
{
	int longestApprox;
	if (e1.getEdgeStrip().rows > e2.getEdgeStrip().rows)
		longestApprox = e1.getEdgeStrip().rows;
	else
		longestApprox = e2.getEdgeStrip().rows;
	Mat edgeSideBySide(longestApprox, e1.getEdgeStrip().cols * 2, CV_8UC3, Scalar(255, 255, 255));
	
	e1.getEdgeStrip().copyTo(edgeSideBySide(Rect(0, 0, e1.getEdgeStrip().cols, e1.getEdgeStrip().rows)));
	flip(e2.getEdgeStrip(), edgeSideBySide(Rect(e1.getEdgeStrip().cols, 0, e2.getEdgeStrip().cols, e2.getEdgeStrip().rows)), 0);
	//
	//namedWindow("sup", WINDOW_NORMAL);
	//namedWindow("gray", WINDOW_NORMAL);
	//namedWindow("singlePixel", WINDOW_NORMAL);
	//namedWindow("singlePixelResult", WINDOW_NORMAL);
	//namedWindow("beforegray", WINDOW_NORMAL);

	//imshow("beforegray", edgeSideBySide);
	cvtColor(edgeSideBySide, edgeSideBySide, CV_BGR2GRAY);
	//imshow("gray", edgeSideBySide);
	
	
	int paletIndex = 4;
	int* paletBinEdge1 = new int[paletIndex];
	int* paletBinEdge2 = new int[paletIndex];
	Mat singlePixelComparison(edgeSideBySide.rows, 2, CV_8UC1);
	
	for (int i = 0; i < paletIndex; i++)
	{
		paletBinEdge1[i] = 0;
		paletBinEdge2[i] = 0;
	}
	
	for (int j = 0; j < edgeSideBySide.rows; j++)
	{
		int valueEdge1 = edgeSideBySide.at<uchar>(Point((edgeSideBySide.cols / 2)-1, j));
		int valueEdge2 = edgeSideBySide.at<uchar>(Point((edgeSideBySide.cols / 2), j));

		for (int k = 0; k < paletIndex; k++)
		{
			if (valueEdge1 < (k + 1) * (256 / paletIndex))
			{
				singlePixelComparison.at<uchar>(Point(0, j)) = k*(256 / paletIndex);
				paletBinEdge1[k]++;
				break;
			}
		}

		for (int k = 0; k < paletIndex; k++)
		{
			if (valueEdge2 < (k + 1) * (256 / paletIndex))
			{
				singlePixelComparison.at<uchar>(Point(1, j)) = k*(256 / paletIndex);
				paletBinEdge2[k]++;
				break;
			}
		}
	}
	//imshow("singlePixel", singlePixelComparison);
	
	
	//Score match
	int mostOccuringBinIndex1 = 0;
	int mostOccuringBinIndex2 = 0;
	bool mostOccuringBinSignificant = false;

	for (int i = 0; i < paletIndex; i++)
	{
		if (paletBinEdge1[i] > paletBinEdge1[mostOccuringBinIndex1])
			mostOccuringBinIndex1 = i;
		if (paletBinEdge2[i] > paletBinEdge1[mostOccuringBinIndex2])
			mostOccuringBinIndex2 = i;
	}
	Mat result(edgeSideBySide.rows, 1, CV_8UC3, Scalar(255,255,255));
	
	//check to see if the most occuring bin is significant in the comparison
	if (mostOccuringBinIndex1 == mostOccuringBinIndex2 && (paletBinEdge1[mostOccuringBinIndex1] > singlePixelComparison.rows *0.5 || paletBinEdge1[mostOccuringBinIndex2] > singlePixelComparison.rows *0.5))
		mostOccuringBinSignificant = true;
	int score = 0;
	int bestMatch, goodMatch, averageMatch, badMatch;

	if (mostOccuringBinSignificant)
	{
		bestMatch = 7;
		goodMatch = 3;
		averageMatch = 2;
		badMatch = 0;
	}

	if (!mostOccuringBinSignificant)
	{
		bestMatch = 2;
		goodMatch = 2;
		averageMatch = 1;
		badMatch = 0;
	}
		
	for (int i = 0; i < singlePixelComparison.rows; i++)
	{
		int value1 = singlePixelComparison.at<uchar>(Point(0, i));
		int value2 = singlePixelComparison.at<uchar>(Point(1, i));

		
		if		(value1 == value2 && (value1 / (256 / paletIndex)) != mostOccuringBinIndex1) //Same bin, not significant bin: best match
		{
			result.at<Vec3b>(Point(0, i)) = {0,255,0};
			score += bestMatch;
		}
		else if (value1 == value2 && (value1 / (256 / paletIndex)) == mostOccuringBinIndex1) //Same bin, significant bin: good match
		{
			result.at<Vec3b>(Point(0, i)) = { 0, 255, 255 };
			score += goodMatch;
		}
		else if ( abs(value1 / (256 / paletIndex) - (value2 / (256 / paletIndex)) )  == 1) //Bin differs by 1: ok match
		{
			result.at<Vec3b>(Point(0, i)) = { 0, 153, 255 };
			score += averageMatch;
		}
		else //bad match
		{
			result.at<Vec3b>(Point(0, i)) = { 0, 0, 255 };
			score += badMatch;
		}
	}
	
	
	
	
	//imshow("sup", edgeSideBySide);
	//imshow("singlePixelResult", result);

	return score;
}