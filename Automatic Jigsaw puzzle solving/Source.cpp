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
#include "globalAlgorithm.h"
#include <dlib/optimization/max_cost_assignment.h>

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
	Mat img1 = imread("C:/Users/g11f0364/Desktop/scanned puzzles/set1black.jpg");
	Mat img2 = imread("C:/Users/g11f0364/Desktop/scanned puzzles/set1white.jpg");
	Mat img3 = imread("C:/Users/g11f0364/Desktop/scanned puzzles/set2black.jpg");
	Mat img4 = imread("C:/Users/g11f0364/Desktop/scanned puzzles/set2white.jpg");
	
	Mat combined1(img1.rows, img1.cols+img3.cols, CV_8UC3, Scalar(255, 255, 255));
	Mat	combined2(img2.rows, img2.cols+img4.cols, CV_8UC3, Scalar(255, 255, 255));

	combined1.copyTo(img);

	img1.copyTo(combined1(Rect(0, 0, img1.cols, img1.rows)));
	img3.copyTo(combined1(Rect(img1.cols, 0, img3.cols, img3.rows)));

	img2.copyTo(combined2(Rect(0, 0, img2.cols, img2.rows)));
	img4.copyTo(combined2(Rect(img2.cols, 0, img4.cols, img4.rows)));

	combined2.copyTo(img);

	Mat gray1, gray2;

	cvtColor(combined1, gray1, CV_BGR2GRAY);
	cvtColor(combined2, gray2, CV_BGR2GRAY);

	namedWindow("Original1", WINDOW_NORMAL);
	imshow("Original1", combined1);
	
	//namedWindow("Original2", WINDOW_NORMAL);
	//imshow("Original2", combined2);

	subtract(gray2, gray1, edited);

	//namedWindow("Original",WINDOW_NORMAL);
	//imshow("Original", img);

	//namedWindow("GrayScale Image");
	//imshow("GrayScale Image", edited);

	threshold(edited, edited, 15, 255, THRESH_BINARY);

	//namedWindow("Threshold Image");
	//imshow("Threshold Image", edited);

	erode(edited, edited, Mat(), Point(-1, -1), 3);
	dilate(edited, edited, Mat(), Point(-1, -1), 3);

	//namedWindow("Erode Dialate Image 2", WINDOW_NORMAL);
	//imshow("Erode Dialate Image 2", edited);

	dilate(edited, edited, Mat(), Point(-1, -1), 2);
	erode(edited, edited, Mat(), Point(-1, -1), 2);

	//namedWindow("Dialate Erode Image", WINDOW_NORMAL);
	//imshow("Dialate Erode Image", edited);

	//img = imread("C:/Users/g11f0364/Desktop/Puzzles/BlackPuzzle.jpg");

	//cvtColor(img, edited, CV_BGR2GRAY);

	//threshold(edited, edited, 15, 255, THRESH_BINARY);

	//namedWindow("Threshold Image");
	//imshow("Threshold Image", edited);

	//erode(edited, edited, Mat());
	//dilate(edited, edited, Mat());

	//namedWindow("Erode Dialate Image", WINDOW_NORMAL);
	//imshow("Erode Dialate Image", edited);

	//dilate(edited, edited, Mat());
	//erode(edited, edited, Mat());

	//namedWindow("Dialate Erode Image", WINDOW_NORMAL);
	//imshow("Dialate Erode Image", edited);

	//erode(edited, edited, Mat(), Point(-1, -1), 2);
	//dilate(edited, edited, Mat(), Point(-1, -1), 2);

	//namedWindow("Erode Dialate Image 2", WINDOW_NORMAL);
	//imshow("Erode Dialate Image 2", edited);


}

void getContoursAndCorners()
{
	vector<vector<Point> > contours;
	
	vector<Vec4i> hierarchy;
	
	findContours(edited, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE, Point(0, 0));
	for (int i = 0; i < contours.size(); i++)
	{
		if (!(fabs(contourArea(contours[i]))>1000 && fabs(contourArea(contours[i])) < 500000))
		{
			contours.erase(contours.begin() + i);
			i = 0;
		}
	}

	for (int i = 0; i < contours.size(); i++)
	{
		reverse(contours[i].begin(), contours[i].end());
	}

	double averageAreaOfPiece = 0;
	
	vector<vector<Point> > polyApprox(contours.size());

	for (int i = 0; i< contours.size(); i++)
	{
		Scalar color = Scalar(255, 255, 0);

		if (fabs(contourArea(contours[i]))>1000 && fabs(contourArea(contours[i])) < 500000)
		{
			approxPolyDP(contours[i], polyApprox[i], 15.0, true);
			//drawContours(img, polyApprox, i, color, 5, 8, hierarchy, 0, Point());
			averageAreaOfPiece += contourArea(contours[i]);
			Piece p(img,i);
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
		circle(img, centroid, 10, cvScalar(255, 0, 255), -1);


		trueCorners = utility_CornerIdentificaion::identifyTrueCorners(potentialCornersV, findCentroid(contours[i]), sqrt(averageAreaOfPiece));
		vector<Point> trueCornersV = { begin(trueCorners), end(trueCorners) };

		if (i == 7)
		{
			cout << "hi";
		}

		puzzleV[i].createEdges(trueCornersV, contours[i]);
		puzzleV[i].centroid = centroid;
		
		
		string word;
		if (puzzleV[i].type == INTERIOR)
		{
			word = "In";
		}

		else if (puzzleV[i].type == CORNER)
		{
			word = "Cnr";
		}

		else if (puzzleV[i].type == FRAME)
		{
			word = "Frm";
		}
		//putText(img, word, Point(centroid.x-25,centroid.y-25), FONT_HERSHEY_SIMPLEX, 2, Scalar(0, 0, 0), 3);
		//putText(img, to_string(i),centroid, FONT_HERSHEY_SIMPLEX, 2, Scalar(0, 0, 0), 3);
		

		int numPotentialCorners = potentialCorners.size();
		for (int j = 0; j < numPotentialCorners; j++)
		{
			//circle(img, potentialCorners.front(), 10, cvScalar(0, 255, 255), 2, 8, 0);
			//putText(img, to_string(j), potentialCorners.front(), FONT_HERSHEY_SIMPLEX, 2, cvScalar(255, 255, 255),3);
			potentialCorners.pop_front();
		}

		for (int j = 0; j < trueCornersV.size(); j++)
		{
			//circle(img, trueCornersV[j], 20, cvScalar(255, 0, 255), 3, 8, 0);
			//putText(img, to_string(j), trueCornersV[j], FONT_HERSHEY_SIMPLEX, 3, cvScalar(0, 0, 0),4);
		}
	}
}

int main()
{
	dlib::matrix<Piece> solvedPuzzle(4,4);
	initialise();
	getContoursAndCorners();
	globalAlgorithm::solvePuzzle(puzzleV, img);

	namedWindow("Contours",WINDOW_NORMAL);
	imshow("Contours", img);

//	sequentialLocalMatching::localMatchImage(puzzleV[26].edges[0], puzzleV[32].edges[2]);

	cvWaitKey(0); //wait for a key press
	//cleaning up
	cvDestroyAllWindows();

	

	return 0;
}