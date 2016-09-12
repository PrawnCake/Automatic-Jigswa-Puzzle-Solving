#include "Edge.h"

using namespace std;
using namespace cv;

enum EdgeType { STRAIGHT = 0, CONVEX = 1, CONCAVE = 2 };

EdgeType edgetype;
vector<Point> edgePoints;
double changeInAngles[];
double lengthBetweenPoints[];
Mat edgeStrip;

Edge::Edge()
{
}


Edge::~Edge()
{
}
