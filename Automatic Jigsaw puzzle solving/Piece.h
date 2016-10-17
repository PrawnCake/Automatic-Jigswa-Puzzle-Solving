#pragma once
#include "Edge.h"

enum PieceType { CORNER = 0, FRAME = 1, INTERIOR = 2 };

class Piece
{
public:
	//Functions
	Piece();
	~Piece();
	Piece(Mat img, int id);		//Constructor
	void determinePieceType();	//Counts the number of straight edges to determine the type of piece
	void createEdges(vector<Point> corners, vector<Point> contours);	//Breaks the overall contour up into its four edges 
	void addEdge(Edge e);		//Instantiates an Edge object once it has been captured

	//Attributes
	vector<Point> fullContour;	//Vector array of every Point along the contour
	Mat image;					//The original input image
	Point centroid;				//Centre point of the piece
	PieceType type;				//Piece type
	bool isInitialised = true;	//Used to keep track of Piece objects that are fully instantiated
	int pieceID;

	Edge * edges = new Edge[4];	//Array of the four edges that make up the piece
	Edge left;					//Used to classify edges based on their orientation relative to the final solution grid after placement
	Edge right;
	Edge up;
	Edge down;
	int edgesInitialised = 0;	//Used to keep track of the number of initialised Edge objects
	int straightEdgeCount = 0;	//Keeps track of the number of straight edges

	
};

