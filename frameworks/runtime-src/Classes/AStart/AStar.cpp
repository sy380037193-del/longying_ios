#include "AStar.h"
#include <algorithm>
#include "cocos2d.h"

bool AStar::init()
{
    this->straightCost = 1.0f;
    this->diagCost = sqrt(2);
    this->floydPath.clear();
    return true;
}

bool AStar::findPath(ANodeGrid* grid)
{
    this->grid = grid;
    this->open = Binary::create();
    this->open->retain();
    this->closed = Binary::create();
    this->closed->retain();

    this->startNode = grid->getStartNode();
    this->endNode = grid->getEndNode();

    this->startNode->g = 0;
    this->startNode->h = diagonal(this->startNode);
    this->startNode->f = this->startNode->g + this->startNode->h;
    return search();
}

AStar::~AStar()
{
    CC_SAFE_RELEASE_NULL(open);
    CC_SAFE_RELEASE_NULL(closed);
    this->floydPath.clear();
}

void AStar::clearSelf()
{
    CC_SAFE_RELEASE_NULL(open);
    CC_SAFE_RELEASE_NULL(closed);
    this->floydPath.clear();
}

bool AStar::search()
{
    ANode* node = this->startNode;
    while (node != this->endNode)
    {
        int startX = std::max(0, node->x - 1);
        int endX = std::min(grid->getNumCols() - 1, node->x + 1);
        int startY = std::max(0, node->y - 1);
        int endY = std::min(grid->getNumRows() - 1, node->y + 1);

        for (int i = startX; i <= endX; i++)
        {
            for (int j = startY; j <= endY; j++)
            {
                ANode* test = grid->getNode(i, j);
                if (test == node || !test->walkable || !isDiagonalWalkable(node, test))
                {
                    continue;
                }
                float cost = straightCost;
                if (!((node->x == test->x) || (node->y == test->y)))
                {
                    cost = diagCost;
                }
                float g = node->g + cost * test->costMultiplier;
                float h = diagonal(test);
                float f = g + h;
                bool isInOpen = open->indexOf(test) != -1;
                bool isInClosed = closed->indexOf(test) != -1;
                if (isInOpen || isInClosed)
                {
                    if (test->f > f)
                    {
                        test->f = f;
                        test->g = g;
                        test->h = h;
                        test->setParent(node);
                        if (isInOpen)
                        {
                            open->updateNode(test);
                        }
                    }
                }
                else
                {
                    test->f = f;
                    test->g = g;
                    test->h = h;
                    test->setParent(node);
                    open->push(test);
                }
            }
        }
        closed->push(node);
        if (open->getLength() == 0) {
            ANode* pNode = NULL;
            int len = closed->getLength();
            for (int i = 0; i < len; i++)
            {
                ANode* node = closed->objOfIndex(i);
                if (pNode == NULL) {
                    pNode = node;
                }
                else {
                    if (pNode->h > node->h || (pNode->h == node->h && pNode->f > node->f)) {
                        pNode = node;
                    }
                }
            }
            buildPathWithNode(pNode);
            return false;
        }
        node = open->shift();
    }
    buildPath();
    return true;
}

void AStar::floyd()
{
    if (floydPath.empty())
    {
        return;
    }
    if (floydPath.size() > 2)
    {
        ANode* vector = ANode::create(0, 0);
        ANode* tempVector = ANode::create(0, 0);
        ANode* nodeD1 = floydPath.back();
        ANode* nodeD2 = floydPath[floydPath.size() - 2];
        floydVector(vector, nodeD1, nodeD2);
        for (int i = floydPath.size() - 3; i >= 0; i--)
        {
            ANode* nodeA1 = floydPath[i + 1];
            ANode* nodeA = floydPath[i];
            floydVector(tempVector, nodeA1, nodeA);
            if (vector->x == tempVector->x && vector->y == tempVector->y)
            {
                floydPath.erase(floydPath.begin() + i + 1);
            }
            else
            {
                vector->x = tempVector->x;
                vector->y = tempVector->y;
            }
        }
    }
    for (int i = floydPath.size() - 1; i >= 0; i--)
    {
        for (int j = 0; j < i - 1; j++)
        {
            ANode* ni = floydPath[i];
            ANode* nj = floydPath[j];
            if (!grid->hasBarrier(ni->x, ni->y, nj->x, nj->y))
            {
                floydPath.erase(floydPath.begin() + j + 1, floydPath.begin() + i);
                i = j;
                break;
            }
        }
    }
}

void AStar::buildPath()
{
    floydPath.clear();
    ANode* node = endNode;
    floydPath.push_back(node);

    while (node != startNode)
    {
        node = node->getParent();
        if (node == nullptr)
        {
            return;
        }
        if (std::find(floydPath.begin(), floydPath.end(), node) != floydPath.end())
        {
            break;
        }
        floydPath.insert(floydPath.begin(), node);
    }
}

void AStar::buildPathWithNode(ANode* target)
{
    floydPath.clear();
    if (target) {
        ANode* node = target;
        floydPath.push_back(node);

        while (node != startNode)
        {
            node = node->getParent();
            if (node == nullptr)
            {
                return;
            }
            if (std::find(floydPath.begin(), floydPath.end(), node) != floydPath.end())
            {
                break;
            }
            floydPath.insert(floydPath.begin(), node);
        }
    }
}

void AStar::floydVector(ANode* target, ANode* n1, ANode* n2)
{
    target->x = n1->x - n2->x;
    target->y = n1->y - n2->y;
}

bool AStar::isDiagonalWalkable(ANode* node1, ANode* node2)
{
    ANode* nearByNode1 = grid->getNode(node1->x, node2->y);
    ANode* nearByNode2 = grid->getNode(node2->x, node1->y);
    if (nearByNode1->walkable || nearByNode2->walkable)
    {
        return true;
    }
    return false;
}

float AStar::manhattan(ANode* node)
{
    float a = std::abs(node->x - endNode->x) * straightCost;
    float b = std::abs(node->y + endNode->y) * straightCost;
    return a + b;
}

float AStar::euclidian(ANode* node)
{
    float dx = node->x - endNode->x;
    float dy = node->y - endNode->y;
    return sqrt(dx * dx + dy * dy) * straightCost;
}

float AStar::diagonal(ANode* node)
{
    float dx = std::abs((node->x - endNode->x));
    float dy = std::abs((node->y - endNode->y));
    float diag = std::min(dx, dy);
    float straight = dx + dy;
    return diagCost * diag + straightCost * (straight - 2 * diag);
}

AVector<ANode*> AStar::getFloydPath()
{
    return floydPath;
}
//A星路径打印
void AStar::printFloydPath()
{
    // CCLOG("----->>>Start");
    // for (auto node : floydPath) {
    //     CCLOG("\t %d, %d", node->x, node->y);
    // }
    // CCLOG("----->>>End");
}
