#pragma once
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "Piece.h"
#include "Edge.h"

#include "utility_CornerIdentificaion.h"

using namespace std;
using namespace cv;
class sequentialLocalMatching
{
public:
	static double localMatchShape(Edge e1, Edge e2);
	static double localMatchImage(Edge e1, Edge e2);
	
};

