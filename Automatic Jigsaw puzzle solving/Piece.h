#pragma once
#include "Edge.h"

enum PieceType { CORNER = 0, FRAME = 1, INTERIOR = 2 };

class Piece
{
public:
	Piece(Mat img, int id);
	void addEdge(Edge e);

	void createEdges(vector<Point> corners, vector<Point> contours);

	int pieceID;
	vector<Point> fullContour;
	bool isInitialised = true;
	
	Edge left;
	Edge right;
	Edge up;
	Edge down;
	
	Mat image;
	void determinePieceType();
	Edge * edges = new Edge[4];
	Point centroid;
	PieceType type;
	int edgesInitialised = 0;
	int straightEdgeCount = 0;

	Piece();
	~Piece();


};

