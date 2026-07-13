#include "ANodeGrid.h"
#include "AMathUtil.h"
#include <cmath>
#include <algorithm>

bool ANodeGrid::init()
{
    return true;
}

ANodeGrid* ANodeGrid::create(int numCols, int numRows)
{
    ANodeGrid* grid = new ANodeGrid();
    if (grid && grid->init()) {
        grid->autorelease();
        grid->setSize(numCols, numRows);
        return grid;
    }
    CC_SAFE_DELETE(grid);
    return nullptr;
}

void ANodeGrid::setSize(int numCols, int numRows)
{
    this->numCols = numCols;
    this->numRows = numRows;

    nodes.clear();

    nodes.resize(numCols);
    for (int i = 0; i < numCols; ++i) {
        nodes[i].resize(numRows, nullptr);
        for (int j = 0; j < numRows; ++j) {
            ANode* node = ANode::create(i, j);
            nodes[i][j] = node;
            node->retain();
        }
    }
}

ANode* ANodeGrid::getNode(int x, int y)
{
    if (x < 0 || x >= numCols || y < 0 || y >= numRows) {
        return nullptr;
    }
    return nodes[x][y];
}

void ANodeGrid::setEndNode(int x, int y)
{
    this->endNode = getNode(x, y);
}

void ANodeGrid::setStartNode(int x, int y)
{
    this->startNode = getNode(x, y);
}

void ANodeGrid::setWalkable(int x, int y, bool value)
{
    ANode* n = getNode(x, y);
    if (n != nullptr) {
        n->walkable = value;
    }
}

bool ANodeGrid::hasBarrier(int startX, int startY, int endX, int endY)
{
    if (startX == endX && startY == endY) {
        return false;
    }

    if (!getNode(endX, endY)->walkable) {
        return true;
    }

    Point p1(startX + 0.5, startY + 0.5);
    Point p2(endX + 0.5, endY + 0.5);
    float distX = std::abs(endX - startX);
    float distY = std::abs(endY - startY);
    bool loopDirection = distX > distY;

    sel_callfuncx lineFunc;

    float i = 0;

    float loopStart = 0;

    float loopEnd = 0;

    std::vector<ANode*> nodesPassed;

    if (loopDirection)
    {
        lineFunc = AMathUtil::getLineFunc(p1, p2, 0);
        loopStart = std::min(startX, endX);
        loopEnd = std::max(startX, endX);
        for (i = loopStart; i <= loopEnd; i++)
        {
            if (i == loopStart)
            {
                i += 0.5;
            }
            float yPos = (*lineFunc)(p1, p2, i);
            nodesPassed = getNodesUnderPoint(i, yPos);
            for (ANode* each : nodesPassed)
            {
                if (!each->walkable)
                {
                    return true;
                }
            }
            if (i == loopStart + 0.5)
            {
                i -= 0.5;
            }
        }
    }
    else
    {
        lineFunc = AMathUtil::getLineFunc(p1, p2, 1);

        loopStart = std::min(startY, endY);
        loopEnd = std::max(startY, endY);

        for (i = loopStart; i <= loopEnd; i++)
        {
            if (i == loopStart)
            {
                i += 0.5;
            }
            float xPos = (*lineFunc)(p1, p2, i);
            nodesPassed = getNodesUnderPoint(xPos, i);
            for (ANode* each : nodesPassed)
            {
                if (!each->walkable)
                {
                    return true;
                }
            }
            if (i == loopStart + 0.5)
            {
                i -= 0.5;
            }
        }

    }
    return false;
}

AVector<ANode*> ANodeGrid::getNodesUnderPoint(float xPos, float yPos, AVector<ANode*> exception)
{
    AVector<ANode*> result;
    bool xIsInt = std::floor(xPos) == xPos;
    bool yIsInt = std::floor(yPos) == yPos;
    if (xIsInt && yIsInt)
    {
        result.push_back(getNode(xPos - 1, yPos - 1));
        result.push_back(getNode(xPos, yPos - 1));
        result.push_back(getNode(xPos - 1, yPos));
        result.push_back(getNode(xPos, yPos));
    }
    else if (xIsInt && !yIsInt)
    {
        result.push_back(getNode(xPos - 1, yPos));
        result.push_back(getNode(xPos, yPos));
    }
    else if (!xIsInt && yIsInt)
    {
        result.push_back(getNode(xPos, yPos - 1));
        result.push_back(getNode(xPos, yPos));
    }
    else
    {
        result.push_back(getNode(xPos, yPos));
    }

    for (auto it = result.begin(); it != result.end();)
    {
        if (std::find(exception.begin(), exception.end(), *it) != exception.end())
        {
            it = result.erase(it);
        }
        else
        {
            ++it;
        }
    }
    return result;
}

ANode* ANodeGrid::findReplacer(ANode* fromNode, ANode* toNode)
{
    ANode* result = nullptr;

    if (toNode->walkable)
    {
        result = toNode;
    }
    else
    {
        int maxLoop = std::max(std::abs(toNode->x - fromNode->x), std::abs(toNode->y - fromNode->y));
        int l = 1;
        float dis = -1;
        while (l <= maxLoop)
        {
            for (int i = -l; i <= l; i++) {
                for (int j = -l; j <= l; j++) {
                    if (std::abs(i) == l || std::abs(j) == l) {
                        ANode* temp = getNode(toNode->x + i, toNode->y + j);
                        if (temp != nullptr && temp->walkable) {
                            if (temp == fromNode) {
                                return nullptr;
                            }
                            else {
                                float d = temp->getDistanceTo(fromNode);
                                if (result == nullptr || dis > d) {
                                    result = temp;
                                    dis = d;
                                }
                            }
                        }
                    }
                }
            }
            if (result != nullptr) {
                return result;
            }
            l++;
        }
    }
    return result;
}

void ANodeGrid::calculateBuriedDepth()
{
    for (int i = 0; i < numCols; i++)
    {
        for (int j = 0; j < numRows; j++)
        {
            ANode* node = getNode(i, j);
            if (node->walkable)
            {
                node->buriedDepth = 0;
            }
            else
            {
                node->buriedDepth = getNodeBuriedDepth(node, std::max(numCols, numRows));
            }
        }
    }
}

int ANodeGrid::getNodeBuriedDepth(ANode* node, int loopCount)
{
    int result = node->walkable ? 0 : 1;
    int l = 1;
    while (l <= loopCount)
    {
        int startX = std::max(node->x - 1, 0);
        int endX = std::min(node->x + 1, numCols - 1);
        int startY = std::max(node->y - 1, 0);
        int endY = std::min(node->y + 1, numRows - 1);

        for (int i = startX; i <= endX; i++)
        {
            for (int j = startY; j <= endY; j++)
            {
                ANode* n = getNode(i, j);
                if (n != node && n->walkable)
                {
                    return result;
                }
            }
        }
        result++;
        l++;
    }
    return result;
}

AVector<ANode*> ANodeGrid::concatArrays(AVector<ANode*>& array1, AVector<ANode*>& array2)
{
    AVector<ANode*> result = array1;
    result.insert(result.end(), array2.begin(), array2.end());
    return result;
}

int ANodeGrid::getNumCols()
{
    return numCols;
}

int ANodeGrid::getNumRows()
{
    return numRows;
}

ANode* ANodeGrid::getEndNode()
{
    return endNode;
}

ANode* ANodeGrid::getStartNode()
{
    return startNode;
}
