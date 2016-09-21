#include "Piece.h"

using namespace std;

Piece::Piece()
{
}

PieceType Piece::getPieceType()
{
	return type;
}

void Piece::determinePieceType()
{
	int count = 0;
	for (int i = 0; i < 4; i++)
	{
		if (edges[i].getEdgeType() == STRAIGHT)
			count++;
	}
	if (count == 1)
		type = FRAME;
	else if (count == 2)
		type = CORNER;
	else
		type = INTERIOR;
}

void Piece::addEdge(Edge e)
{
	edges[edgesInitialised] = e;
	edges[edgesInitialised].determineEdgeType();
	edgesInitialised++;
}

void Piece::createEdges(vector<Point> corners, vector<Point> contours)
{

	list<Point> pointsOnEdge;

	for (int i = 0; i < 4; i++)
	{
		pointsOnEdge.clear();
		for (int j = 0; j < contours.size(); j++)
		{
			if (contours[j].x == corners[i].x && contours[j].y == corners[i].y)
			{
				while (!(contours[j].x == corners[(i + 1) % 4].x && contours[j].y == corners[(i + 1) % 4].y))
				{
					pointsOnEdge.push_back(contours[j]);
					j = (j + 1) % contours.size();
				}
				pointsOnEdge.push_back(contours[j]);
				break;
			}
		}
		vector<Point> pointsOnEdgeV = { begin(pointsOnEdge), end(pointsOnEdge) };
		vector<Point> actualPointsOnEdgeV = { begin(pointsOnEdge), end(pointsOnEdge) };
		approxPolyDP(pointsOnEdgeV, pointsOnEdgeV,2.5,false);
		addEdge(Edge(pointsOnEdgeV, actualPointsOnEdgeV));
	}
	determinePieceType();
}

void Piece::setType(PieceType p)
{
	type = p;
}

void Piece::setCentroid(Point c)
{
	centroid = c;
}

Edge Piece::getEdge(int i)
{
	return edges[i];
}

Piece::~Piece()
{
}
