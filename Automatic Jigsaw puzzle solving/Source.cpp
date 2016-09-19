#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "Piece.h"
#include "Edge.h"
#include "sequentialLocalMatching.h"
#include "utility_CornerIdentificaion.h"

using namespace std;
using namespace cv;

Mat img;
Mat edited;
list<Piece> puzzle;
vector<Piece> puzzleV;


Point findCentroid(vector<Point> contour)
{
	double x = 0, y = 0;
	for (int i = 0; i < contour.size(); i++)
	{
		x += contour[i].x;
		y += contour[i].y;
	}
	return Point((int)x / contour.size(), (int)y / contour.size());
}

void initialise()
{
	img = imread("C:/Users/g11f0364/Desktop/Puzzles/BlackPuzzle.jpg");
	//namedWindow("Original");
	//imshow("Original", img);

	cvtColor(img, edited, CV_BGR2GRAY);

	//namedWindow("GrayScale Image");
	//imshow("GrayScale Image", imgGrayScale);

	threshold(edited, edited, 10, 255, THRESH_BINARY);

	//namedWindow("Threshold Image");
	//imshow("Threshold Image", imgGrayScale);

	erode(edited, edited, Mat());
	dilate(edited, edited, Mat());

	//namedWindow("Erode Dialate Image");
	//imshow("Erode Dialate Image", imgGrayScale);

	dilate(edited, edited, Mat());
	erode(edited, edited, Mat());

	//namedWindow("Dialate Erode Image");
	//imshow("Dialate Erode Image", imgGrayScale);

	erode(edited, edited, Mat(), Point(-1, -1), 2);
	dilate(edited, edited, Mat(), Point(-1, -1), 2);

	//namedWindow("Erode Dialate Image 2");
	//imshow("Erode Dialate Image 2", imgGrayScale);
}

void getContoursAndCorners()
{
	vector<vector<Point> > contours;
	
	vector<Vec4i> hierarchy;
	
	findContours(edited, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE, Point(0, 0));
	
	double averageAreaOfPiece = 0;
	
	vector<vector<Point> > polyApprox(contours.size());

	for (int i = 0; i< contours.size(); i++)
	{
		Scalar color = Scalar(255, 255, 0);

		if (fabs(contourArea(contours[i]))>100 && fabs(contourArea(contours[i])) < 500000)
		{
			approxPolyDP(contours[i], polyApprox[i], 5.0, true);
			//drawContours(img, polyApprox, i, color, 2, 8, hierarchy, 0, Point());
			averageAreaOfPiece += contourArea(contours[i]);
			Piece p;
			puzzle.push_back(p);
		}
	}
	averageAreaOfPiece = averageAreaOfPiece / contours.size();

	puzzleV = { begin(puzzle), end(puzzle) };
	
	for (int i = 0; i < contours.size(); i++)
	{
		list<Point> potentialCorners, trueCorners;
		potentialCorners = utility_CornerIdentificaion::identifyCorners(polyApprox[i]);
		vector<Point> potentialCornersV = { begin(potentialCorners), end(potentialCorners) };
		Point centroid = findCentroid(contours[i]);

		trueCorners = utility_CornerIdentificaion::identifyTrueCorners(potentialCornersV, findCentroid(contours[i]), sqrt(averageAreaOfPiece));
		vector<Point> trueCornersV = { begin(trueCorners), end(trueCorners) };

		puzzleV[i].createEdges(trueCornersV, contours[i]);

		circle(img, centroid, 10, cvScalar(255, 0, 255), -1);
		
		string word;
		if (puzzleV[i].getPieceType() == INTERIOR)
		{
			word = "In";
		}

		else if (puzzleV[i].getPieceType() == CORNER)
		{
			word = "Cnr";
		}

		else if (puzzleV[i].getPieceType() == FRAME)
		{
			word = "Frm";
		}
		putText(img, to_string(i),centroid, FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 3);
		putText(img, word, Point(centroid.x-25,centroid.y-25), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 3);

		int numPotentialCorners = potentialCorners.size();
		for (int j = 0; j < numPotentialCorners; j++)
		{
			//circle(img, potentialCorners.front(), 10, cvScalar(0, 255, 255), 2, 8, 0);
			//putText(img, to_string(j), potentialCorners.front(), FONT_HERSHEY_SIMPLEX, 1, cvScalar(255, 255, 255));
			potentialCorners.pop_front();

		}

		int numTrueCorners = trueCorners.size();
		for (int j = 0; j < numTrueCorners; j++)
		{
			circle(img, trueCorners.front(), 10, cvScalar(255, 0, 255), 2, 8, 0);
			putText(img, to_string(j), trueCorners.front(), FONT_HERSHEY_SIMPLEX, 1, cvScalar(255, 255, 255));
			trueCorners.pop_front();
		}
	}
}

int main()
{
	initialise();
	getContoursAndCorners();


	//for (int i = 0; i < 4; i++)
	//{
	//	for (int j = 0; j < 4; j++)
	//	{
	//		if (!(sequentialLocalMatching::localMatchShape(puzzleV[9].getEdge(i), puzzleV[7].getEdge(j)) == 500.0 || sequentialLocalMatching::localMatchShape(puzzleV[9].getEdge(i), puzzleV[7].getEdge(j)) == 600.0))
	//			cout <<"Area of piece 9 edge " + to_string(i) +" and piece 7 edge "+ to_string(j) + ": " << sequentialLocalMatching::localMatchShape(puzzleV[9].getEdge(i),puzzleV[7].getEdge(j)) <<"\n";
	//	}
	//}
	double num = sequentialLocalMatching::localMatchShape(puzzleV[0].getEdge(0), puzzleV[6].getEdge(2));
	cout << "Area of piece 0 edge " + to_string(2) + " and piece 6 edge " + to_string(3) + ": " << num << "\n";
	

	for (int i = 0; i < puzzleV.size(); i++)
	{
		Piece p = puzzleV[i];
		for (int j = 0; j < 4; j++)
		{
			vector<Point> v = p.getEdge(j).getContour();
			for (int k = 0; k < v.size()-1; k++)
			{
//				line(img, Point(v[k].x, v[k].y), Point(v[(k + 1)].x, v[(k + 1)].y), Scalar(0,255,0), 2);
			}
		}
	}

	Piece p1 = puzzleV[0];
	
	vector<Point> v1 = p1.getEdge(0).getContour();
	for (int k = 0; k < v1.size() - 1; k++)
	{
		line(img, Point(v1[k].x, v1[k].y), Point(v1[(k + 1)].x, v1[(k + 1)].y), Scalar(0,255,0), 2);
	}
	

	Piece p2 = puzzleV[6];
	
	vector<Point> v2 = p2.getEdge(2).getContour();
	for (int k = 0; k < v2.size() - 1; k++)
	{
		line(img, Point(v2[k].x, v2[k].y), Point(v2[(k + 1)].x, v2[(k + 1)].y), Scalar(0,255,0), 2);
	}
	

	namedWindow("Contours");
	imshow("Contours", img);

	cvWaitKey(0); //wait for a key press
	//cleaning up
	cvDestroyAllWindows();

	

	return 0;
}