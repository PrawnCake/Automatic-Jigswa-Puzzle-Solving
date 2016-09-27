#pragma once
#include "Edge.h"

enum PieceType { CORNER = 0, FRAME = 1, INTERIOR = 2 };

class Piece
{
public:
	Piece(Mat img, int id);
	void addEdge(Edge e);
	void setEdge(Edge e, int index);
	void setType(PieceType p);
	void setCentroid(Point c);
	void createEdges(vector<Point> corners, vector<Point> contours);
	PieceType getPieceType();
	Edge getEdge(int i);
	int pieceID;
	~Piece();
private:
	Mat image;
	void determinePieceType();
	Edge * edges = new Edge[4];
	Point centroid;
	PieceType type;
	int edgesInitialised = 0;
	int straightEdgeCount = 0;

};

