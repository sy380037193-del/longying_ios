#pragma once
#include "Binary.h"
#include "ANodeGrid.h"
#include "AMathUtil.h"
#include <vector>
#include <cmath>
#include "cocos2d.h"
#include "AVector.h"

class AStar : public cocos2d::Ref
{
private:
    Binary* open;
    Binary* closed;
    ANodeGrid* grid;
    ANode* endNode;
    ANode* startNode;
    AVector<ANode*> floydPath;
    float straightCost;
    float diagCost;
    void buildPath();
    void buildPathWithNode(ANode* target);
    void floydVector(ANode* target, ANode* n1, ANode* n2);
    bool isDiagonalWalkable(ANode* node1, ANode* node2);
    float manhattan(ANode* node);
    float euclidian(ANode* node);
    float diagonal(ANode* node);

public:
    CREATE_FUNC(AStar);
    ~AStar();
    bool init();
    bool findPath(ANodeGrid* grid);
    bool search();

    /** form http://wonderfl.net/c/aWCe **/
    void floyd();
    AVector<ANode*> getFloydPath();
    void printFloydPath();
    void clearSelf();
};
