#include "globalAlgorithm.h"
using namespace std;

int length = 3;
int bredth = 4;
dlib::matrix<Piece> solvedPuzzle(length, bredth);


void placeBestPiece(vector<vector<double>> scores, vector<int> edgesToMatchIndex, vector<Point> pockets)
{
	vector<int> indexOfBestMatchedPiecePerPocket(scores.size());
	vector<double> ratioOfBestMatchedPiecePerPocket(scores.size());

	//find the best piece per pocket, i = pocket, j = piece (note 4 orientations per piece)
	for (int i = 0; i < scores.size(); i++)
	{
		double bestScore = 0;
		double secondBestScore = 0;
		int bestScoreIndex = 0;

		
		for (int j = 0; j < scores[i].size(); j++)
		{
			if (scores[i][j] > bestScore)
			{
				secondBestScore = bestScore;
				bestScore = scores[i][j];
				bestScoreIndex = j;
			}
		}
		if (secondBestScore == 0)
		{
			for (int j = 0; j < scores[i].size(); j++)
			{
				if (scores[i][j] > secondBestScore && scores[i][j] != bestScore)
				{
					secondBestScore = scores[i][j];
				}
			}
		}
		indexOfBestMatchedPiecePerPocket[i] = bestScoreIndex;
		ratioOfBestMatchedPiecePerPocket[i] = bestScore / secondBestScore;
	}

	int highestRatio = 0;
	int pocketWithHighestRatioIndex = 0;
	int indexOfPieceWithinPocket = 0;
	for (int i = 0; i < indexOfBestMatchedPiecePerPocket.size(); i++)
	{
		if (ratioOfBestMatchedPiecePerPocket[i] > highestRatio)
		{
			highestRatio = ratioOfBestMatchedPiecePerPocket[i];
			pocketWithHighestRatioIndex = i;
			indexOfPieceWithinPocket = indexOfBestMatchedPiecePerPocket[i];
		}
	}
	//TODO: WORK OUT WHICH DAMN PIECE AND ITS ORIENTATION ARE PLACED
	Piece p;
}

Edge getTheEdge(Piece thePiece, int edge)
{
	if (thePiece.getPieceType() == FRAME)
		return thePiece.getEdge(2);
	
	if (edge == 0)
		return thePiece.up;
	if (edge == 1)
		return thePiece.left;
	if (edge == 2)
		return thePiece.down;
	if (edge == 3)
		return thePiece.right;
}

double matchTwoEdges(Edge e1, Edge e2, vector<int> edgesToMatch, Point pocket)
{
	Edge firstEdge, secondEdge;

	if (edgesToMatch[0] == 0 && edgesToMatch[1] == 1)
	{
		firstEdge	= getTheEdge(solvedPuzzle(pocket.x, pocket.y + 1), 0);
		secondEdge	= getTheEdge(solvedPuzzle(pocket.x + 1, pocket.y), 1);
	}
	
	else if (edgesToMatch[0] == 1 && edgesToMatch[1] == 2)
	{
		firstEdge	= getTheEdge(solvedPuzzle(pocket.x + 1, pocket.y), 1);
		secondEdge	= getTheEdge(solvedPuzzle(pocket.x, pocket.y - 1), 2);
	}

	else if (edgesToMatch[0] == 2 && edgesToMatch[1] == 3)
	{
		firstEdge	= getTheEdge(solvedPuzzle(pocket.x, pocket.y - 1), 2);
		secondEdge	= getTheEdge(solvedPuzzle(pocket.x - 1, pocket.y), 3);
	}

	else if (edgesToMatch[0] == 3 && edgesToMatch[1] == 0)
	{
		firstEdge	= getTheEdge(solvedPuzzle(pocket.x - 1, pocket.y), 3);
		secondEdge	= getTheEdge(solvedPuzzle(pocket.x, pocket.y + 1), 0);
	}

	return sequentialLocalMatching::localMatchShape(e1, firstEdge) + sequentialLocalMatching::localMatchShape(e2, secondEdge);
}

vector<int> getNeighbouringEdgesIndex2(Point pocket)
{
	Piece p1 = solvedPuzzle(pocket.x - 1, pocket.y);
	Piece p2 = solvedPuzzle(pocket.x, pocket.y + 1);

	if (p1.isInitialised && p2.isInitialised)
	{
		return vector<int> {3,0};
	}

	p1 = solvedPuzzle(pocket.x, pocket.y + 1);
	p2 = solvedPuzzle(pocket.x + 1, pocket.y);

	if (p1.isInitialised && p2.isInitialised)
	{
		return vector<int> {0, 1};
	}

	p1 = solvedPuzzle(pocket.x + 1, pocket.y);
	p2 = solvedPuzzle(pocket.x, pocket.y - 1);

	if (p1.isInitialised && p2.isInitialised)
	{
		return vector<int> {1, 2};
	}

	p1 = solvedPuzzle(pocket.x - 1, pocket.y);
	p2 = solvedPuzzle(pocket.x, pocket.y - 1);

	if (p1.isInitialised && p2.isInitialised)
	{
		return vector<int> {2, 3};
	}
}

//vector<Edge> getNeighbouringEdgesIndex3(Point pocket)
//{
//	//find the empty edge
//
//	vector<Edge> edges(3);
//	if (!solvedPuzzle(pocket.x - 1, pocket.y).isInitialised) //missing piece is on left
//	{
//		//edges[0] = getLeftEdgeFromNeightbour();
//		//edges[1] = getEdgeFromNeightbour();
//		//edges[2] = getEdgeFromNeightbour();
//	}
//	else if (!solvedPuzzle(pocket.x, pocket.y + 1).isInitialised) // missing piece is below
//	{
//
//	}
//	else if (!solvedPuzzle(pocket.x + 1, pocket.y).isInitialised) // missing piece is right
//	{
//
//	}
//	else if (!solvedPuzzle(pocket.x, pocket.y - 1).isInitialised) //missing piece is above
//	{
//
//	}
//}

vector<vector<Point>> findAllPockets()
{
	list<Point> fourSidePockets;
	list<Point> threeSidePockets;
	list<Point> twoSidePockets;
	for (int i = 0; i < length; i++)
	{
		for (int j = 0; j < bredth; j++)
		{
			int neighbourCount = 0;
			if (!solvedPuzzle(i, j).isInitialised)
			{
				neighbourCount =
					solvedPuzzle(i - 1, j).isInitialised +
					solvedPuzzle(i + 1, j).isInitialised +
					solvedPuzzle(i, j - 1).isInitialised +
					solvedPuzzle(i, j + 1).isInitialised;
			}

			if (neighbourCount == 2)
				twoSidePockets.push_back(Point(i, j));
			else if (neighbourCount == 3)
				threeSidePockets.push_back(Point(i, j));
			else if (neighbourCount == 4)
				fourSidePockets.push_back(Point(i, j));
		}
	}
	vector<vector<Point>> allPockets = { { begin(fourSidePockets), end(fourSidePockets) }, { begin(threeSidePockets), end(threeSidePockets) }, { begin(twoSidePockets), end(twoSidePockets) } };
	return allPockets;
}

void placeInteriorPieces(vector<Piece> interiorPieces)
{
	vector<vector<Point>> allPockets = findAllPockets();

	vector<Point> fourSidePockets = allPockets[0];
	vector<Point> threeSidePockets = allPockets[1];
	vector<Point> twoSidePockets = allPockets[2];

	if (fourSidePockets.size() > 0) //last piece to be placed
	{
		//TODO
		return;
	}

	else if (threeSidePockets.size() > 0) //last piece to be placed
	{
		//vector<vector<double>> pocketScores(threeSidePockets.size());
		//
		//for (int i = 0; i<threeSidePockets.size(); i++)
		//{
		//	Point pocket = threeSidePockets[i];
		//	vector<Edge> edgesToMatch = getNeighbouringEdges3(pocket);
		//}


	}

	else if (twoSidePockets.size() > 0) //last piece to be placed
	{
		vector<vector<double>> pocketScores(twoSidePockets.size());
		for (int i = 0; i < twoSidePockets.size(); i++)
		{
			pocketScores[i] = vector<double>(interiorPieces.size());
		}
		vector<int> edgesToMatchIndex;
		for (int i = 0; i<twoSidePockets.size(); i++)
		{
			Point pocket = twoSidePockets[i];

			//this is used to determine which edges (in terms of up down left right) are matched, as currently the orientation of the interior piece relative to the frame is unknown
			edgesToMatchIndex = getNeighbouringEdgesIndex2(pocket); 

			for (int j = 0; j < interiorPieces.size(); j++)
			{
				for (int k = 0; k < 4; k++)
				{
					pocketScores[i][j*4+k] = matchTwoEdges(interiorPieces[j].getEdge(k), interiorPieces[j].getEdge((k + 1) % 4), edgesToMatchIndex, pocket);
				}
			}
		}

		placeBestPiece(pocketScores, edgesToMatchIndex, twoSidePockets);
	}
}

dlib::matrix<int> generateScoreMatrixForFramePieces(vector<Piece> framePieces)
{

	dlib::matrix<int> matrix(framePieces.size(), framePieces.size());

	for (int i = 0; i < framePieces.size(); i++)
	{
		for (int j = 0; j < framePieces.size(); j++)
		{
			if (i == j) //same piece
			{
				matrix(i, j) = 1;
				continue;
			}

			Edge e1;
			if (framePieces[i].getPieceType() == FRAME)
			{
				e1 = framePieces[i].getEdge(3);
			}
			else if (framePieces[i].getPieceType() == CORNER)
			{
				e1 = framePieces[i].getEdge(2);
			}

			Edge e2 = framePieces[j].getEdge(1);




			if (e1.getEdgeType() == e2.getEdgeType())
			{
				matrix(i, j) = 0;
				continue;
			}


			double shapeScore = sequentialLocalMatching::localMatchShape(e1, e2);
			//double imageScore = 1000 - sequentialLocalMatching::localMatchImage(e1, e2);
			double imageScore = 0;
			matrix(i, j) = 5000 - (int)(shapeScore + imageScore);
		}
	}
	return matrix;
}

void placeFramePieces(vector<Piece> framePieces)
{
	dlib::matrix<int> matrix = generateScoreMatrixForFramePieces(framePieces);
	vector<long> optimalAssignmentOfFramePieces = dlib::max_cost_assignment(matrix);

	//place frame pieces
	int changeX = 0;
	int changeY = -1;
	int x = 0;
	int y = 0;
	int firstCornerIndex;
	for (int i = 0; i < framePieces.size(); i++) // find first corner
	{
		if (framePieces[i].getPieceType() == CORNER)
		{
			firstCornerIndex = i;
			break;
		}
	}

	int index = firstCornerIndex;
	do
	{
		solvedPuzzle(x, y) = framePieces[index];

		if (framePieces[index].getPieceType() == CORNER) // change direction
		{
			if (changeX == 0 && changeY == -1)
			{
				changeX = 1;
				changeY = 0;
			}
			else if (changeX == 1 && changeY == 0)
			{
				changeX = 0;
				changeY = 1;
			}
			else if (changeX == 0 && changeY == 1)
			{
				changeX = -1;
				changeY = 0;
			}
			else if (changeX == -1 && changeY == 0)
			{
				changeX = 0;
				changeY = -1;
			}
		}

		index = optimalAssignmentOfFramePieces[index];
		x += changeX;
		y += changeY;
	} while (index != firstCornerIndex);
}

void alignFramePieces(vector<Piece> pieces)
{
	for (int i = 0; i < pieces.size(); i++)
	{
		Piece p = pieces[i];
		if (p.getPieceType() != FRAME)
			continue;
		int indexOfFlatEdge = 0;
		for (indexOfFlatEdge = 0; indexOfFlatEdge < 4; indexOfFlatEdge++)
		{
			Edge e = p.getEdge(indexOfFlatEdge);
			if (e.getEdgeType() == STRAIGHT)
				break;
		}
		//shift edges to the left to make the flat edge be edge 0
		for (int j = 0; j < indexOfFlatEdge; j++)
		{
			Edge tmp = p.getEdge(0);
			p.setEdge(p.getEdge(1), 0);
			p.setEdge(p.getEdge(2), 1);
			p.setEdge(p.getEdge(3), 2);
			p.setEdge(tmp, 3);
		}
	}
}

void alignCornerPieces(vector<Piece> pieces)
{
	for (int i = 0; i < pieces.size(); i++)
	{
		Piece p = pieces[i];
		if (p.getPieceType() != CORNER)
			continue;
		int indexOfLastFlatEdge = 0;

		for (indexOfLastFlatEdge = 0; indexOfLastFlatEdge < 4; indexOfLastFlatEdge++)
		{
			Edge e = p.getEdge(indexOfLastFlatEdge);

			if (indexOfLastFlatEdge == 0 && e.getEdgeType() == STRAIGHT)
			{
				if (p.getEdge(3).getEdgeType() == STRAIGHT)  //orientation is correct, break
					break;
				else if (p.getEdge(1).getEdgeType() == STRAIGHT) //one left switch needed
				{
					indexOfLastFlatEdge = 1;
					break;
				}
			}

			if (e.getEdgeType() == STRAIGHT)
			{
				indexOfLastFlatEdge++;
				break;
			}
		}

		for (int j = 0; j < indexOfLastFlatEdge; j++)
		{
			Edge tmp = pieces[i].getEdge(0);
			pieces[i].setEdge(p.getEdge(1), 0);
			pieces[i].setEdge(p.getEdge(2), 1);
			pieces[i].setEdge(p.getEdge(3), 2);
			pieces[i].setEdge(tmp, 3);
		}
	}
}

dlib::matrix<Piece> globalAlgorithm::solvePuzzle(vector<Piece> pieces, Mat img)
{
	alignCornerPieces(pieces);
	alignFramePieces(pieces);
	list<Piece> framePiecesL;
	list<Piece> interiorPiecesL;
	for (int i = 0; i < pieces.size(); i++)
	{
		if (pieces[i].getPieceType() != INTERIOR)
			framePiecesL.push_back(pieces[i]);
		else 
			interiorPiecesL.push_back(pieces[i]);
	}

	vector<Piece> framePieces = { begin(framePiecesL), end(framePiecesL) };
	vector<Piece> interiorPieces = { begin(interiorPiecesL), end(interiorPiecesL) };

	placeFramePieces(framePieces);
	placeInteriorPieces(interiorPieces);
	
	
	int singleBlockDimention = 260;
	Mat completePuzzleGrid(singleBlockDimention * 4, singleBlockDimention * 4, CV_8UC3, Scalar(255, 255, 255));

	for (int i = 0; i < 3; i++)
	{
		Point startOfBlock;
		for (int j = 0; j < 4; j++)
		{
			if (!solvedPuzzle(i, j).isInitialised)
				continue;
			startOfBlock.x = solvedPuzzle(i, j).getCentroid().x - 130;
			startOfBlock.y = solvedPuzzle(i, j).getCentroid().y - 130;
			if (startOfBlock.x < 0)
				startOfBlock.x = 0;
			if (startOfBlock.y < 0)
				startOfBlock.y = 0;

			img(Rect(startOfBlock.x, startOfBlock.y, 260, 260)).copyTo(completePuzzleGrid(Rect(i*singleBlockDimention, j*singleBlockDimention, singleBlockDimention, singleBlockDimention)));
		}
	}
	imshow("final", completePuzzleGrid);
	
	return solvedPuzzle;
}
