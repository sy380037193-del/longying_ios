#pragma once
#include "ANode.h"

class Binary : public cocos2d::Ref
{
public:
	/**
	* @param compareValue 排序字段，若为空字符串则直接比较被添加元素本身的值
	*
	*/
	CREATE_FUNC(Binary);
	bool init();

	/** 向二叉堆中添加元素
	* @param node 欲添加的元素对象
	*/
	void push(ANode *node);
	/** 弹出开启列表中第一个元素 */
	ANode *shift();
	/** 更新某一个节点的值。在你改变了二叉堆中某一节点的值以后二叉堆不会自动进行排序，所以你需要手动
	*  调用此方法进行二叉树更新 */
	void updateNode(ANode *node);
	/** 查找某节点所在索引位置 */
	int indexOf(ANode *node);
    ANode *objOfIndex(unsigned int index);
	int getLength();
	/**比较两个节点，返回true则表示第一个节点小于第二个*/
	bool compareTwoNodes(ANode *node1, ANode *node2);

	/** 写此方法的目的在于快速trace出所需要查看的结果，直接trace一个Binary对象即可得到其中全部元素的值或
	* 排序字段的值 */
	const char* toString();
    
private:
	std::vector<ANode*> data;
};