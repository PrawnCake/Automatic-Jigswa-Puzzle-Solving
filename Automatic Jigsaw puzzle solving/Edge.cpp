#include "Edge.h"

using namespace std;
using namespace cv;



EdgeType edgetype;
vector<Point> edgePoints;
double changeInAngles[];
double lengthBetweenPoints[];
Mat edgeStrip;

Edge::Edge()
{

}

Edge::Edge(vector<Point> e)
{
	edgePoints = e;
}


void Edge::calcChangeInAngles()
{
	//TODO
}

void Edge::lengthBetweenPoints()
{
	//TODO
}

Edge::~Edge()
{
}
