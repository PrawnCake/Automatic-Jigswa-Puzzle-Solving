#pragma once
#include "Piece.h"
#include "sequentialLocalMatching.h"
#include <dlib/optimization/max_cost_assignment.h>"

class globalAlgorithm
{
public:
	static void alignFramePieces(vector<Piece> pieces);
	static void alignCornerPieces(vector<Piece> pieces);
	static dlib::matrix<int> generateScoreMatrixForFramePieces(vector<Piece> pieces);
};

