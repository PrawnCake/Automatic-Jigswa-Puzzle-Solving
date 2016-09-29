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
#include <dlib/optimization/max_cost_assignment.h>"

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
	namedWindow("Original",WINDOW_NORMAL);
	imshow("Original", img);

	cvtColor(img, edited, CV_BGR2GRAY);

	//namedWindow("GrayScale Image",WINDOW_NORMAL);
	//imshow("GrayScale Image", edited);

	threshold(edited, edited, 15, 255, THRESH_BINARY);

	//namedWindow("Threshold Image", WINDOW_NORMAL);
	//imshow("Threshold Image", edited);

	erode(edited, edited, Mat());
	dilate(edited, edited, Mat());

	//namedWindow("Erode Dialate Image", WINDOW_NORMAL);
	//imshow("Erode Dialate Image", edited);

	dilate(edited, edited, Mat());
	erode(edited, edited, Mat());

	//namedWindow("Dialate Erode Image", WINDOW_NORMAL);
	//imshow("Dialate Erode Image", edited);

	erode(edited, edited, Mat(), Point(-1, -1), 2);
	dilate(edited, edited, Mat(), Point(-1, -1), 2);

	//namedWindow("Erode Dialate Image 2", WINDOW_NORMAL);
	//imshow("Erode Dialate Image 2", edited);
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

		trueCorners = utility_CornerIdentificaion::identifyTrueCorners(potentialCornersV, findCentroid(contours[i]), sqrt(averageAreaOfPiece));
		vector<Point> trueCornersV = { begin(trueCorners), end(trueCorners) };

		puzzleV[i].createEdges(trueCornersV, contours[i]);
		puzzleV[i].setCentroid(centroid);
		//circle(img, centroid, 10, cvScalar(255, 0, 255), -1);
		
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
		//putText(img, word, Point(centroid.x-25,centroid.y-25), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 3);

		int numPotentialCorners = potentialCorners.size();
		for (int j = 0; j < numPotentialCorners; j++)
		{
			//circle(img, potentialCorners.front(), 10, cvScalar(0, 255, 255), 2, 8, 0);
			//putText(img, to_string(j), potentialCorners.front(), FONT_HERSHEY_SIMPLEX, 1, cvScalar(255, 255, 255));
			potentialCorners.pop_front();

		}
	}
}

int main()
{
	dlib::matrix<Piece> solvedPuzzle(4,4);
	initialise();
	getContoursAndCorners();
	globalAlgorithm::solvePuzzle(puzzleV, img);


	//for (int i = 0; i < puzzleV.size(); i++)
	//{
	//	for (int j = 0; j < 4; j++)
	//	{
	//		putText(img, to_string(j), puzzleV[i].getEdge(j).getActualContour()[0], FONT_HERSHEY_SIMPLEX, 1, cvScalar(255, 255, 255));
	//	}
	//}
	
	//list<Piece> framePiecesL;
	//for (int i = 0; i < puzzleV.size(); i++)
	//{
	//	if (puzzleV[i].getPieceType() != INTERIOR)
	//		framePiecesL.push_back(puzzleV[i]);
	//}

	//vector<Piece> framePieces = { begin(framePiecesL), end(framePiecesL) };
	//dlib::matrix<int> matrix = globalAlgorithm::generateScoreMatrixForFramePieces(framePieces);

	//vector<long> optimalAssignment = dlib::max_cost_assignment(matrix);

	//
	////place frame pieces
	//int changeX = 0;
	//int changeY = -1;
	//int x = 0;
	//int y = 0;
	//int firstCornerIndex;
	//for (int i = 0; i < framePieces.size(); i++) // find first corner
	//{
	//	if (framePieces[i].getPieceType() == CORNER)
	//	{
	//		firstCornerIndex = i;
	//		break;
	//	}
	//}

	//int index = firstCornerIndex;
	//do
	//{
	//	solvedPuzzle(x, y) = framePieces[index];

	//	if (framePieces[index].getPieceType() == CORNER) // change direction
	//	{
	//		if (changeX == 0 && changeY == -1)
	//		{
	//			changeX = 1;
	//			changeY = 0;
	//		}
	//		else if (changeX == 1 && changeY == 0)
	//		{
	//			changeX = 0;
	//			changeY = 1;
	//		}
	//		else if (changeX == 0 && changeY == 1)
	//		{
	//			changeX = -1;
	//			changeY = 0;
	//		}
	//		else if (changeX == -1 && changeY == 0)
	//		{
	//			changeX = 0;
	//			changeY = -1;
	//		}
	//	}

	//	index = optimalAssignment[index];
	//	x += changeX;
	//	y += changeY;
	//} 
	//while (index != firstCornerIndex);
	//
	//int singleBlockDimention = 260;
	//Mat completePuzzleGrid(singleBlockDimention*4, singleBlockDimention*4, CV_8UC3, Scalar(255, 255, 255));

	//for (int i = 0; i < 3; i++)
	//{
	//	Point startOfBlock;
	//	for (int j = 0; j < 4; j++)
	//	{
	//		if (!solvedPuzzle(i, j).isInitialised)
	//			continue;
	//		startOfBlock.x = solvedPuzzle(i, j).getCentroid().x - 130;
	//		startOfBlock.y = solvedPuzzle(i, j).getCentroid().y - 130;
	//		if (startOfBlock.x < 0)
	//			startOfBlock.x = 0;
	//		if (startOfBlock.y < 0)
	//			startOfBlock.y = 0;
	//		
	//		img(Rect(startOfBlock.x, startOfBlock.y, 260, 260)).copyTo(completePuzzleGrid(Rect(i*singleBlockDimention, j*singleBlockDimention, singleBlockDimention, singleBlockDimention)));
	//	}
	//}
	//imshow("final", completePuzzleGrid);

	double min = 10000.0;
	int piece;
	int edge;

	int maxMatch;
	int maxMatchPieceIndex;
	int maxMatchEdgeIndex;
	
	//for (int k = 0; k < puzzleV.size(); k++)
	//{
	//	for (int l = 0; l < 4; l++)
	//	{
	//		maxMatch = 0;
	//		maxMatchPieceIndex = 0;
	//		maxMatchEdgeIndex = 0;

	//		if (puzzleV[k].getEdge(l).getEdgeType() == STRAIGHT)
	//			continue;
	//		for (int i = 0; i < puzzleV.size(); i++)
	//		{
	//			if (i == k)
	//				continue;
	//			for (int j = 0; j < 4; j++)
	//			{
	//				if (puzzleV[i].getEdge(j).getEdgeType() == STRAIGHT)
	//					continue;
	//				double score = sequentialLocalMatching::localMatchImage(puzzleV[k].getEdge(l), puzzleV[i].getEdge(j), img);
	//				if (score > maxMatch)
	//				{
	//					maxMatch = score;
	//					maxMatchPieceIndex = i;
	//					maxMatchEdgeIndex = j;
	//				}
	//			}
	//		}
	//		cout << "Piece : "<< k << " Edge: "<< l << " is best matched to Piece: " << maxMatchPieceIndex << " Edge: " << maxMatchEdgeIndex << endl;
	//	}
	//}


//	for (int i = 1; i < puzzleV.size(); i++)
//	{
//		for (int j = 0; j < 4; j++)
//		{
//			double area = sequentialLocalMatching::localMatchShape(puzzleV[11].getEdge(1), puzzleV[i].getEdge(j));
//			cout << "Area of piece 11 edge " + to_string(1) + " and piece " << i <<" edge " + to_string(j) + ": " << area << "\n";
//			if (area < min)
//			{
//				min = area;
//				piece = i;
//				edge = j;
//			}
//		}
//	}
//	cout << "Piece: " << piece << " Edge: " << edge << endl;
//	//double num = sequentialLocalMatching::localMatchShape(puzzleV[0].getEdge(0), puzzleV[6].getEdge(2));
//	//cout << "Area of piece 0 edge " + to_string(2) + " and piece 6 edge " + to_string(3) + ": " << num << "\n";
//	//
//
//	for (int i = 0; i < puzzleV.size(); i++)
//	{
//		Piece p = puzzleV[i];
//		for (int j = 0; j < 4; j++)
//		{
//			vector<Point> v = p.getEdge(j).getContour();
//			for (int k = 0; k < v.size()-1; k++)
//			{
////				line(img, Point(v[k].x, v[k].y), Point(v[(k + 1)].x, v[(k + 1)].y), Scalar(0,255,0), 2);
//			}
//		}
//	}

	namedWindow("Contours");
	imshow("Contours", img);

	cvWaitKey(0); //wait for a key press
	//cleaning up
	cvDestroyAllWindows();

	

	return 0;
}