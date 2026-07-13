#include "ANode.h"

bool ANode::init()
{
	this->costMultiplier = 1.0f;
	this->walkable = true;
	this->buriedDepth = -1;
	this->parent = NULL;
	return true;
}

ANode* ANode::create(int x, int y)
{
	ANode* node = ANode::create();
	node->x = x;
	node->y = y;
	return node;
}

/**
* 判断两个节点屏幕位置是否一样
* @param otherNode
* @return
*
*/
bool ANode::posEquals(ANode *otherNode)
{
	if (this->posX == otherNode->posX && this->posY == otherNode->posY)
	{
		return true;
	}
	return false;
}

/** 得到此节点到另一节点的网格距离 */
float ANode::getDistanceTo(ANode *targetNode)
{
	float disX = targetNode->x - this->x;
	float disY = targetNode->y - this->y;
	this->distance = sqrtf(disX*disX + disY*disY);
	return this->distance;
}

ANode* ANode::getParent()
{
	return parent;
}

void ANode::setParent(ANode* node)
{
	this->parent = node;
}