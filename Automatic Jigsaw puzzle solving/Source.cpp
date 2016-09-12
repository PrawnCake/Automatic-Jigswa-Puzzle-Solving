#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "utility_CornerIdentificaion.h"

using namespace std;
using namespace cv;

int main()
{
	Mat img = imread("C:/Users/g11f0364/Desktop/Puzzles/BlackPuzzle.jpg");
	//namedWindow("Original");
	//imshow("Original", img);

	Mat imgGrayScale; 
	cvtColor(img, imgGrayScale, CV_BGR2GRAY);

	//namedWindow("GrayScale Image");
	//imshow("GrayScale Image", imgGrayScale);

	threshold(imgGrayScale, imgGrayScale, 10, 255, THRESH_BINARY);

	//namedWindow("Threshold Image");
	//imshow("Threshold Image", imgGrayScale);

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;


	erode(imgGrayScale, imgGrayScale, Mat());
	dilate(imgGrayScale, imgGrayScale, Mat());

	//namedWindow("Erode Dialate Image");
	//imshow("Erode Dialate Image", imgGrayScale);

	dilate(imgGrayScale, imgGrayScale, Mat());
	erode(imgGrayScale, imgGrayScale, Mat());

	//namedWindow("Dialate Erode Image");
	//imshow("Dialate Erode Image", imgGrayScale);

	erode(imgGrayScale, imgGrayScale, Mat(),Point(-1,-1),2);
	dilate(imgGrayScale, imgGrayScale, Mat(),Point(-1, -1), 2);

	//namedWindow("Erode Dialate Image 2");
	//imshow("Erode Dialate Image 2", imgGrayScale);

	findContours(imgGrayScale, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	vector<vector<Point> > polyApprox(contours.size());
	for (int i = 0; i< contours.size(); i++)
	{
		Scalar color = Scalar(255,255,0);
		
		if (fabs(contourArea(contours[i]))>100 && fabs(contourArea(contours[i])) < 500000)
		{
			approxPolyDP(contours[i], polyApprox[i], 5.0, true);
			drawContours(img, polyApprox, i, color, 2, 8, hierarchy, 0, Point());
		}	
	}

	for (int i = 0; i < polyApprox.size(); i++)
	{
		list<Point> potentialCorners;
		potentialCorners = utility_CornerIdentificaion::identifyCorners(polyApprox[i]);

		int numPotentialCorners = potentialCorners.size();
		for (int j = 0; j < numPotentialCorners; j++)
		{
			circle(img, potentialCorners.front(), 10, cvScalar(0, 255, 255), 2, 8, 0);
			potentialCorners.pop_front();
		}
	}

	namedWindow("Contours");
	imshow("Contours", img);

	cvWaitKey(0); //wait for a key press

	//cleaning up
	cvDestroyAllWindows();

	return 0;
}