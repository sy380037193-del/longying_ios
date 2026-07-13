#pragma once
#include "ANode.h"
#include "AVector.h"

class ANodeGrid : public cocos2d::Ref
{
public:
	int top;
	int bottom;
	static ANodeGrid *create(int numCols, int numRows);
	bool init();
	CREATE_FUNC(ANodeGrid);
	void setSize(int numCols, int numRows);
	ANode* getNode(int x, int y);
	void setEndNode(int x, int y);
	void setStartNode(int x, int y);
	void setWalkable(int x, int y, bool value);
	bool hasBarrier(int staratX, int starttY, int endX, int endY);
	AVector<ANode*> getNodesUnderPoint(float xPos, float yPos, AVector<ANode*> exception = {});
	ANode *findReplacer(ANode *fromNode, ANode *toNode);
	void calculateBuriedDepth();
	ANode* getEndNode();
	int getNumCols();
	int getNumRows();
	ANode* getStartNode();
private:
	AVector<AVector<ANode*>> nodes;
	ANode* startNode;
	ANode* endNode;
	int numCols;
	int numRows;
	int getNodeBuriedDepth(ANode *node, int loopCount = 10);
	AVector<ANode*> concatArrays(AVector<ANode*>& array1, AVector<ANode*>& array2);
};