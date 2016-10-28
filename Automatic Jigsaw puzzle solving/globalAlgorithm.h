#pragma once
#include "Piece.h"
#include "sequentialLocalMatching.h"
#include <dlib/optimization/max_cost_assignment.h>"
#include "utility_CornerIdentificaion.h"

class globalAlgorithm
{
public:
	static dlib::matrix<Piece> solvePuzzle(vector<Piece> pieces, Mat img);
};

