#include "parallelLocalMatching.h"

extern "C" int runParallelMatch(int *edge1, int *edge2, int dataSize);

int parallelLocalMatching::calcScoreParallel(Edge e1, Edge e2)
{
	int shortestStripLength;
	int shortestSide;

	if (e1.edgetype == e2.edgetype)
		return -10000;

	if (e1.edgeStrip.rows < e2.edgeStrip.rows)
	{
		shortestStripLength = e1.edgeStrip.rows;
		shortestSide = 1;
	}
	else
	{
		shortestStripLength = e2.edgeStrip.rows;
		shortestSide = 2;
	}

	int difference = abs(e1.edgeStrip.rows - e2.edgeStrip.rows);


	Mat edgeSideBySide(shortestStripLength, e1.edgeStrip.cols * 2, CV_8UC3, Scalar(255, 255, 255));

	if (shortestSide == 1)
	{
		e1.edgeStrip.copyTo(edgeSideBySide(Rect(0, 0, e1.edgeStrip.cols, e1.edgeStrip.rows)));
		Mat tmp = e2.edgeStrip(Rect(0, (int)(round((double)(difference) / 2)), e1.edgeStrip.cols, shortestStripLength));
		flip(tmp, edgeSideBySide(Rect(e1.edgeStrip.cols, 0, e2.edgeStrip.cols, shortestStripLength)), 0);
	}

	else if (shortestSide == 2)
	{
		e2.edgeStrip.copyTo(edgeSideBySide(Rect(0, 0, e2.edgeStrip.cols, e2.edgeStrip.rows)));
		Mat tmp = e1.edgeStrip(Rect(0, (int)(round((double)(difference) / 2)), e2.edgeStrip.cols, shortestStripLength));
		flip(tmp, edgeSideBySide(Rect(e2.edgeStrip.cols, 0, e1.edgeStrip.cols, shortestStripLength)), 0);
	}
	

	vector<int> e1Vec;
	vector<int> e2Vec;
	int *e1Arr;
	int *e2Arr;
	for (int i = 0; i < edgeSideBySide.rows; i++)
	{

	}
	
	return 1;
}