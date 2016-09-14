#include "Piece.h"



Edge * edges = new Edge[4];
Point centroid;
PieceType type;
int edgesInitialised = 0;

using namespace std;

Piece::Piece()
{
}

void Piece::addEdge(Edge e)
{
	edges[edgesInitialised++] = e;
}

void Piece::createEdges(vector<Point> corners, vector<Point> contours)
{

	list<Point> pointsOnEdge;

	for (int i = 0; i < 4; i++)
	{
		list<Point> pointsOnEdge;
		for (int j = 0; j < contours.size(); j++)
		{
			if (contours[j].x == corners[i].x && contours[j].y == corners[i].y)
			{
				while (!(contours[j].x == corners[(i + 1) % 4].x && contours[j].y == corners[(i + 1) % 4].y))
				{
					j = (j + 1) % contours.size();
					pointsOnEdge.push_back(contours[j]);
				}
				pointsOnEdge.push_back(contours[j]);
				break;
			}
		}
		vector<Point> pointsOnEdgeV = { begin(pointsOnEdge), end(pointsOnEdge) };
		addEdge(Edge(pointsOnEdgeV));
	}
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
