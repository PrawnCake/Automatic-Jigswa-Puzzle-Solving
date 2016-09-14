#pragma once
#include "Edge.h"

enum PieceType { CORNER = 0, EDGE = 1, INTERIOR = 2 };

class Piece
{
public:
	Piece();
	void addEdge(Edge e);
	void setType(PieceType p);
	void setCentroid(Point c);
	void createEdges(vector<Point> corners, vector<Point> contours);
	Edge getEdge(int i);
	~Piece();
};

