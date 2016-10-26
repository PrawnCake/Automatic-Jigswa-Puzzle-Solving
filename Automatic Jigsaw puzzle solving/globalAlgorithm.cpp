#include "globalAlgorithm.h"

using namespace std;

const int length = 5;
const int bredth = 7;
dlib::matrix<Piece> solvedPuzzle(length, bredth);


double scoreTwoEdges(Edge e1, Edge e2)
{
	//double shapeScore = sequentialLocalMatching::localMatchShape(e1, e2)*0.01;
	double shapeScore = 0;
	double imageScore = sequentialLocalMatching::localMatchImage(e1, e2);
	//double imageScore = parallelLocalMatching::localMatchImage(e1, e2);
	//double imageScore = 100000;

	return imageScore - shapeScore;
}

int placeBestPiece(vector<vector<double>> scores, vector<vector<int>> edgesToMatchIndexList, vector<Point> pockets, vector<Piece> pieces)
{
	vector<int> indexOfBestMatchedPiecePerPocket(scores.size());
	vector<double> ratioOfBestMatchedPiecePerPocket(scores.size());

	//find the best piece per pocket, i = pocket, j = piece (note 4 orientations per piece)
	for (int i = 0; i < scores.size(); i++)
	{
		double bestScore = -10000;
		double secondBestScore = -10000;
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
		cout << "Piece matched: " << pieces[bestScoreIndex/4].pieceID << endl;
	}

	double highestRatio = 0;
	int pocketWithHighestRatioIndex = 0;
	int indexOfPiece = 0;
	for (int i = 0; i < indexOfBestMatchedPiecePerPocket.size(); i++)
	{
		if (ratioOfBestMatchedPiecePerPocket[i] > highestRatio)
		{
			highestRatio = ratioOfBestMatchedPiecePerPocket[i];
			pocketWithHighestRatioIndex = i;
			indexOfPiece = indexOfBestMatchedPiecePerPocket[i];
		}
	}
	//Place piece
	Piece p = pieces[indexOfPiece/4];

	int orientation = indexOfPiece % 4;

	//orientate Piece
	if (edgesToMatchIndexList[pocketWithHighestRatioIndex][0] == 0)
	{
		p.down	= p.edges[orientation];
		p.right = p.edges[(orientation + 1) % 4];
		p.up	= p.edges[(orientation + 2) % 4];
		p.left	= p.edges[(orientation + 3) % 4];
	}

	else if (edgesToMatchIndexList[pocketWithHighestRatioIndex][0] == 1)
	{
		p.right = p.edges[orientation];
		p.up	= p.edges[(orientation + 1) % 4];
		p.left	= p.edges[(orientation + 2) % 4];
		p.down	= p.edges[(orientation + 3) % 4];
	}

	else if (edgesToMatchIndexList[pocketWithHighestRatioIndex][0] == 2)
	{
		p.up	= p.edges[orientation];
		p.left	= p.edges[(orientation + 1) % 4];
		p.down	= p.edges[(orientation + 2) % 4];
		p.right = p.edges[(orientation + 3) % 4];
	}

	else if (edgesToMatchIndexList[pocketWithHighestRatioIndex][0] == 3)
	{
		p.left	= p.edges[orientation];
		p.down	= p.edges[(orientation + 1) % 4];
		p.right = p.edges[(orientation + 2) % 4];
		p.up	= p.edges[(orientation + 3) % 4];
	}
	solvedPuzzle(pockets[pocketWithHighestRatioIndex].x, pockets[pocketWithHighestRatioIndex].y) = p;
	return indexOfPiece / 4;
}

Edge getTheEdge(Piece thePiece, int edge)
{
	if (thePiece.type == FRAME)
		return thePiece.edges[2];
	
	if (edge == 0)
		return thePiece.down;
	if (edge == 1)
		return thePiece.left;
	if (edge == 2)
		return thePiece.up;
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

	return (scoreTwoEdges(e1, firstEdge) + scoreTwoEdges(e2, secondEdge));
}

double matchThreeEdges(Edge e1, Edge e2, Edge e3, vector<int> edgesToMatch, Point pocket)
{
	Edge firstEdge, secondEdge, thirdEdge;

	if (edgesToMatch[0] == 0 && edgesToMatch[1] == 1 && edgesToMatch[2] == 2)
	{
		firstEdge	= getTheEdge(solvedPuzzle(pocket.x, pocket.y + 1), 0);
		secondEdge	= getTheEdge(solvedPuzzle(pocket.x + 1, pocket.y), 1);
		thirdEdge	= getTheEdge(solvedPuzzle(pocket.x, pocket.y - 1), 2);
	}

	else if (edgesToMatch[0] == 1 && edgesToMatch[1] == 2 && edgesToMatch[2] == 3)
	{
		firstEdge	= getTheEdge(solvedPuzzle(pocket.x + 1, pocket.y), 1);
		secondEdge	= getTheEdge(solvedPuzzle(pocket.x, pocket.y - 1), 2);
		thirdEdge	= getTheEdge(solvedPuzzle(pocket.x - 1, pocket.y), 3);
	}

	else if (edgesToMatch[0] == 2 && edgesToMatch[1] == 3 && edgesToMatch[2] == 0)
	{
		firstEdge	= getTheEdge(solvedPuzzle(pocket.x, pocket.y - 1), 2);
		secondEdge	= getTheEdge(solvedPuzzle(pocket.x - 1, pocket.y), 3);
		thirdEdge	= getTheEdge(solvedPuzzle(pocket.x, pocket.y + 1), 0);
	}

	else if (edgesToMatch[0] == 3 && edgesToMatch[1] == 0 && edgesToMatch[2] == 1)
	{
		firstEdge	= getTheEdge(solvedPuzzle(pocket.x - 1, pocket.y), 3);
		secondEdge	= getTheEdge(solvedPuzzle(pocket.x, pocket.y + 1), 0);
		thirdEdge	= getTheEdge(solvedPuzzle(pocket.x + 1, pocket.y), 1);
	}
	double score1 = scoreTwoEdges(e1, firstEdge);
	double score2 = scoreTwoEdges(e2, secondEdge);
	double score3 = scoreTwoEdges(e3, thirdEdge);
	return (score1 + score2 + score3);
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

	p1 = solvedPuzzle(pocket.x, pocket.y - 1);
	p2 = solvedPuzzle(pocket.x - 1, pocket.y);

	if (p1.isInitialised && p2.isInitialised)
	{
		return vector<int> {2, 3};
	}
}

vector<int> getNeighbouringEdgesIndex3(Point pocket)
{
	Piece p1 = solvedPuzzle(pocket.x - 1, pocket.y);
	Piece p2 = solvedPuzzle(pocket.x, pocket.y + 1);
	Piece p3 = solvedPuzzle(pocket.x + 1, pocket.y);

	if (p1.isInitialised && p2.isInitialised && p3.isInitialised)
	{
		return vector<int> {3, 0, 1};
	}

	p1 = solvedPuzzle(pocket.x, pocket.y + 1);
	p2 = solvedPuzzle(pocket.x + 1, pocket.y);
	p3 = solvedPuzzle(pocket.x, pocket.y - 1);


	if (p1.isInitialised && p2.isInitialised && p3.isInitialised)
	{
		return vector<int> {0, 1, 2};
	}

	p1 = solvedPuzzle(pocket.x + 1, pocket.y);
	p2 = solvedPuzzle(pocket.x, pocket.y - 1);
	p3 = solvedPuzzle(pocket.x - 1, pocket.y);

	if (p1.isInitialised && p2.isInitialised && p3.isInitialised)
	{
		return vector<int> {1, 2, 3};
	}

	p1 = solvedPuzzle(pocket.x, pocket.y - 1);
	p2 = solvedPuzzle(pocket.x - 1, pocket.y);
	p3 = solvedPuzzle(pocket.x, pocket.y + 1);

	if (p1.isInitialised && p2.isInitialised && p3.isInitialised)
	{
		return vector<int> {2, 3, 0};
	}
}


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
				if (i - 1 < 0 || i + 1 >= length || j - 1 < 0 || j + 1 >= bredth)
					continue;
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
	
	int blehbleh = 0;
	while (!(blehbleh++ ==15))
	{
		vector<vector<Point>> allPockets = findAllPockets();

		vector<Point> fourSidePockets = allPockets[0];
		vector<Point> threeSidePockets = allPockets[1];
		vector<Point> twoSidePockets = allPockets[2];
	
		if (fourSidePockets.size() > 0) //last piece to be placed
		{
			solvedPuzzle(fourSidePockets[0].x, fourSidePockets[0].y) = interiorPieces[0];
			interiorPieces.erase(interiorPieces.begin());
			return;
		}

		else if (threeSidePockets.size() > 0)
		{
			vector<vector<double>> pocketScores(threeSidePockets.size());
			for (int i = 0; i < threeSidePockets.size(); i++)
			{
				pocketScores[i] = vector<double>(interiorPieces.size() * 4);
			}
			vector<vector<int>> edgesToMatchIndexList;
			vector<int> edgesToMatchIndex;
			for (int i = 0; i < threeSidePockets.size(); i++)
			{
				Point pocket = threeSidePockets[i];

				//this is used to determine which edges (in terms of up down left right) are matched, as currently the orientation of the interior piece relative to the frame is unknown
				edgesToMatchIndex = getNeighbouringEdgesIndex3(pocket);
				edgesToMatchIndexList.push_back(edgesToMatchIndex);
				for (int j = 0; j < interiorPieces.size(); j++)
				{
					for (int k = 0; k < 4; k++)
					{
						double score = matchThreeEdges(interiorPieces[j].edges[k], interiorPieces[j].edges[(k + 1) % 4], interiorPieces[j].edges[(k + 2) % 4], edgesToMatchIndex, pocket);
						pocketScores[i][j * 4 + k] = score;
						cout << "Piece: " << interiorPieces[j].pieceID << " Orientation: " << k << " Score: " << score << endl;
					}
				}
			}

			int piecePlacedIndex = placeBestPiece(pocketScores, edgesToMatchIndexList, threeSidePockets, interiorPieces);
			interiorPieces.erase(interiorPieces.begin() + piecePlacedIndex);
		}

		else if (twoSidePockets.size() > 0)
		{
			vector<vector<double>> pocketScores(twoSidePockets.size());
			for (int i = 0; i < twoSidePockets.size(); i++)
			{
				pocketScores[i] = vector<double>(interiorPieces.size() * 4);
			}
			vector<int> edgesToMatchIndex;
			vector<vector<int>> edgesToMatchIndexList;
			for (int i = 0; i < twoSidePockets.size(); i++)
			{
				Point pocket = twoSidePockets[i];

				//this is used to determine which edges (in terms of up down left right) are matched, as currently the orientation of the interior piece relative to the frame is unknown
				edgesToMatchIndex = getNeighbouringEdgesIndex2(pocket);
				edgesToMatchIndexList.push_back(edgesToMatchIndex);
				for (int j = 0; j < interiorPieces.size(); j++)
				{
					for (int k = 0; k < 4; k++)
					{
						double score = matchTwoEdges(interiorPieces[j].edges[k], interiorPieces[j].edges[(k + 1) % 4], edgesToMatchIndex, pocket);
						pocketScores[i][j * 4 + k] = score;
						cout << "Piece: " << interiorPieces[j].pieceID << " Orientation: " << k << " Score: " << score << endl;
					}
				}
			}

			int piecePlacedIndex = placeBestPiece(pocketScores, edgesToMatchIndexList, twoSidePockets, interiorPieces);
			interiorPieces.erase(interiorPieces.begin() + piecePlacedIndex);
		}
	}
}

dlib::matrix<int> generateScoreMatrixForFramePieces(vector<Piece> framePieces)
{
	const int fp = framePieces.size();
	dlib::matrix<int> matrix(fp, fp);

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
			if (framePieces[i].type == FRAME)
			{
				e1 = framePieces[i].edges[3];
			}
			else if (framePieces[i].type == CORNER)
			{
				e1 = framePieces[i].edges[2];
			}

			Edge e2 = framePieces[j].edges[1];




			if (e1.edgetype == e2.edgetype)
			{
				matrix(i, j) = 0;
				continue;
			}


			//double shapeScore = sequentialLocalMatching::localMatchShape(e1, e2)*0.01;
			double shapeScore = 0;
			double imageScore = sequentialLocalMatching::localMatchImage(e1, e2);
			//double imageScore = 100000;
			matrix(i, j) = imageScore - shapeScore;
		}
	}
	return matrix;
}

void placeFramePieces(vector<Piece> framePieces)
{
	const dlib::matrix<int> matrix = generateScoreMatrixForFramePieces(framePieces);
	vector<long> optimalAssignmentOfFramePieces = dlib::max_cost_assignment(matrix);

	//place frame pieces
	int changeX = 0;
	int changeY = -1;
	int x = 0;
	int y = 0;
	int firstCornerIndex;
	for (int i = 0; i < framePieces.size(); i++) // find first corner
	{
		if (framePieces[i].type == CORNER)
		{
			firstCornerIndex = i;
			break;
		}
	}

	int index = firstCornerIndex;
	do
	{
		solvedPuzzle(x, y) = framePieces[index];

		if (framePieces[index].type == CORNER) // change direction
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
		if (p.type != FRAME)
			continue;
		int indexOfFlatEdge = 0;
		for (indexOfFlatEdge = 0; indexOfFlatEdge < 4; indexOfFlatEdge++)
		{
			Edge e = p.edges[indexOfFlatEdge];
			if (e.edgetype == STRAIGHT)
				break;
		}
		//shift edges to the left to make the flat edge be edge 0
		for (int j = 0; j < indexOfFlatEdge; j++)
		{
			Edge tmp = p.edges[0];
			p.edges[0] = p.edges[1];
			p.edges[1] = p.edges[2];
			p.edges[2] = p.edges[3];
			p.edges[3] = tmp;
		}
	}
}

void alignCornerPieces(vector<Piece> pieces)
{
	for (int i = 0; i < pieces.size(); i++)
	{
		Piece p = pieces[i];
		if (p.type != CORNER)
			continue;
		int indexOfLastFlatEdge = 0;

		for (indexOfLastFlatEdge = 0; indexOfLastFlatEdge < 4; indexOfLastFlatEdge++)
		{
			Edge e = p.edges[indexOfLastFlatEdge];

			if (indexOfLastFlatEdge == 0 && e.edgetype == STRAIGHT)
			{
				if (p.edges[3].edgetype == STRAIGHT)  //orientation is correct, break
					break;
				else if (p.edges[1].edgetype == STRAIGHT) //one left switch needed
				{
					indexOfLastFlatEdge = 1;
					break;
				}
			}

			if (e.edgetype == STRAIGHT)
			{
				indexOfLastFlatEdge++;
				break;
			}
		}

		for (int j = 0; j < indexOfLastFlatEdge; j++)
		{
			Edge tmp = pieces[i].edges[0];
			pieces[i].edges[0] = p.edges[1];
			pieces[i].edges[1] = p.edges[2];
			pieces[i].edges[2] = p.edges[3];
			pieces[i].edges[3] = tmp;
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
		if (pieces[i].type != INTERIOR)
			framePiecesL.push_back(pieces[i]);
		else 
			interiorPiecesL.push_back(pieces[i]);
	}

	vector<Piece> framePieces = { begin(framePiecesL), end(framePiecesL) };
	vector<Piece> interiorPieces = { begin(interiorPiecesL), end(interiorPiecesL) };

	placeFramePieces(framePieces);
	placeInteriorPieces(interiorPieces);
	
	int singleBlockDimention = 320;
	Mat completePuzzleGrid(singleBlockDimention * (bredth), singleBlockDimention * (length), CV_8UC3, Scalar(255, 255, 255));

	for (int i = 0; i < length; i++)
	{
		Point startOfBlock;
		for (int j = 0; j < bredth; j++)
		{
			if (!solvedPuzzle(i, j).isInitialised)
				continue;
			startOfBlock.x = solvedPuzzle(i, j).centroid.x - 175;
			startOfBlock.y = solvedPuzzle(i, j).centroid.y - 175;
			if (startOfBlock.x < 0)
				startOfBlock.x = 0;
			if (startOfBlock.y < 0)
				startOfBlock.y = 0;

			img(Rect(startOfBlock.x, startOfBlock.y, 320, 320)).copyTo(completePuzzleGrid(Rect(i*singleBlockDimention, j*singleBlockDimention, singleBlockDimention, singleBlockDimention)));
		}
	}
	namedWindow("final",WINDOW_NORMAL);
	imshow("final", completePuzzleGrid);
	
	return solvedPuzzle;
}
