#include "parallelLocalMatching.h"

extern "C" int* runParallelMatch(int *edge1, int *edge2, int dataSize, bool signifBin, int bin);

int parallelLocalMatching::localMatchImage(Edge e1, Edge e2)
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
	
	int paletIndex = 8;
	int* paletBinEdge1 = new int[paletIndex];
	int* paletBinEdge2 = new int[paletIndex];
	Mat singlePixelComparison(edgeSideBySide.rows, 2, CV_8UC1);

	for (int i = 0; i < paletIndex; i++)
	{
		paletBinEdge1[i] = 0;
		paletBinEdge2[i] = 0;
	}

	for (int j = 0; j < edgeSideBySide.rows; j++)
	{
		int valueEdge1 = edgeSideBySide.at<uchar>(Point((edgeSideBySide.cols / 2) - 1, j));
		int valueEdge2 = edgeSideBySide.at<uchar>(Point((edgeSideBySide.cols / 2), j));

		for (int k = 0; k < paletIndex; k++)
		{
			if (valueEdge1 < (k + 1) * (256 / paletIndex))
			{
				singlePixelComparison.at<uchar>(Point(0, j)) = k*(256 / paletIndex);
				paletBinEdge1[k]++;
				break;
			}
		}

		for (int k = 0; k < paletIndex; k++)
		{
			if (valueEdge2 < (k + 1) * (256 / paletIndex))
			{
				singlePixelComparison.at<uchar>(Point(1, j)) = k*(256 / paletIndex);
				paletBinEdge2[k]++;
				break;
			}
		}
	}

	int mostOccuringBinIndex1 = 0;
	int mostOccuringBinIndex2 = 0;
	bool mostOccuringBinSignificant = false;

	for (int i = 0; i < paletIndex; i++)
	{
		if (paletBinEdge1[i] > paletBinEdge1[mostOccuringBinIndex1])
			mostOccuringBinIndex1 = i;
		if (paletBinEdge2[i] > paletBinEdge1[mostOccuringBinIndex2])
			mostOccuringBinIndex2 = i;
	}

	if (mostOccuringBinIndex1 == mostOccuringBinIndex2 && (paletBinEdge1[mostOccuringBinIndex1] > singlePixelComparison.rows *0.5 && paletBinEdge1[mostOccuringBinIndex2] > singlePixelComparison.rows *0.5))
		mostOccuringBinSignificant = true;

	vector<int> e1Vec;
	vector<int> e2Vec;

	for (int i = 0; i < edgeSideBySide.rows; i++)
	{
		e1Vec.push_back(edgeSideBySide.at<uchar>(Point(0, i)));
		e2Vec.push_back(edgeSideBySide.at<uchar>(Point(1, i)));
	}
	int *e1Arr = &e1Vec[0];
	int *e2Arr = &e2Vec[0];

	int *score;

	if (mostOccuringBinSignificant)
	{
		score = runParallelMatch(e1Arr, e2Arr, edgeSideBySide.rows, true, mostOccuringBinIndex1);
	}
	else
	{
		score = runParallelMatch(e1Arr, e2Arr, edgeSideBySide.rows, false, mostOccuringBinIndex1);
	}

	int total = 0;

	for (int i = 0; i < edgeSideBySide.rows; i++)
	{
		total += score[i];
	}

	return total;
}