#include "globalAlgorithm.h"
using namespace std;


void globalAlgorithm::alignFramePieces(vector<Piece> pieces)
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

//TO COMPLETE
void globalAlgorithm::alignCornerPieces(vector<Piece> pieces)
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

dlib::matrix<int> globalAlgorithm::generateScoreMatrixForFramePieces(vector<Piece> pieces)
{
	list<Piece> framePiecesL;

	for (int i = 0; i < pieces.size(); i++)
	{
		if (pieces[i].getPieceType() != INTERIOR)
		framePiecesL.push_back(pieces[i]);
	}

	vector<Piece> framePieces = { begin(framePiecesL), end(framePiecesL) };
	
	dlib::matrix<int> matrix(framePieces.size(), framePieces.size());

	for (int i = 0; i < framePieces.size(); i++)
	{
		for (int j = 0; j < framePieces.size(); j++)
		{
			if (i == j) //same piece
			{
				matrix(i,j) = 1;
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
