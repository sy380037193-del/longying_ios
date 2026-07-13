#include "Binary.h"
#include <algorithm>

/**
* @param compareValue 排序字段，若为空字符串则直接比较被添加元素本身的值
*/
bool Binary::init()
{
    data.clear();
    return true;
}

/** 向二叉堆中添加元素
* @param node 欲添加的元素对象
*/
void Binary::push(ANode* node)
{
    // 将新节点添加到末尾
    data.push_back(node);
    int len = data.size();

    // 若数组中只有一个元素则省略排序过程，否则对新元素执行上浮过程
    if (len > 1)
    {
        // 新添入节点当前所在索引
        int index = len - 1;
        // 新节点当前父节点所在索引
        int parentIndex = (index - 1) / 2;
        ANode* temp;

        // 和它的父节点比较，如果新元素比父节点元素小则交换这两个元素
        while (index > 0 && compareTwoNodes(node, data[parentIndex]))
        {
            temp = data[parentIndex];
            data[parentIndex] = node;
            data[index] = temp;
            index = parentIndex;
            parentIndex = (index - 1) / 2;
        }
    }
}

/** 弹出开启列表中第一个元素 */
ANode* Binary::shift()
{
    // 先弹出列首元素
    if (data.empty())
    {
        return nullptr;
    }

    ANode* result = data.front();
    data.erase(data.begin());

    // 若弹出列首元素后数组空了则省略排序过程
    if (data.empty())
    {
        return result;
    }

    // 将列尾元素排至首位
    ANode* lastNode = data.back();
    data.pop_back();
    data.insert(data.begin(), lastNode);

    // 末尾节点当前所在索引
    int index = 0;
    // 末尾节点当前第一子节点所在索引
    int childIndex = 2 * index + 1;
    // 末尾节点当前两个子节点中较小的一个的索引
    int compareIndex = 0;
    ANode* temp = nullptr;

    // 和它的两个子节点比较，如果较小的子节点比它小就将它们交换
    while (childIndex < data.size())
    {
        // 只有一个子节点的情况
        if (childIndex + 1 == data.size())
        {
            compareIndex = childIndex;
        }
        // 有两个子节点则取其中较小的那个
        else
        {
            compareIndex = compareTwoNodes(data[childIndex], data[childIndex + 1]) ? childIndex : childIndex + 1;
        }

        if (compareTwoNodes(data[compareIndex], lastNode))
        {
            temp = data[compareIndex];
            data[compareIndex] = lastNode;
            data[index] = temp;
            index = compareIndex;
            childIndex = 2 * index + 1;
        }
        else
        {
            break;
        }
    }

    return result;
}

/** 更新某一个节点的值 */
void Binary::updateNode(ANode* node)
{
    auto it = std::find(data.begin(), data.end(), node);
    if (it == data.end())
    {
        return;
    }

    int index = std::distance(data.begin(), it);
    int parentIndex = (index - 1) / 2;
    ANode* temp = nullptr;
    int compareIndex = 0;

    // 上浮过程
    while (index > 0 && compareTwoNodes(node, data[parentIndex]))
    {
        temp = data[parentIndex];
        data[parentIndex] = node;
        data[index] = temp;
        index = parentIndex;
        parentIndex = (index - 1) / 2;
    }

    // 下沉过程
    int childIndex = 2 * index + 1;
    while (childIndex < data.size())
    {
        // 只有一个子节点的情况
        if (childIndex + 1 == data.size())
        {
            compareIndex = childIndex;
        }
        // 有两个子节点则取其中较小的那个
        else
        {
            compareIndex = compareTwoNodes(data[childIndex], data[childIndex + 1]) ? childIndex : childIndex + 1;
        }

        if (compareTwoNodes(data[compareIndex], node))
        {
            temp = data[compareIndex];
            data[compareIndex] = node;
            data[index] = temp;
            index = compareIndex;
            childIndex = 2 * index + 1;
        }
        else
        {
            break;
        }
    }
}

int Binary::indexOf(ANode* node)
{
    auto it = std::find(data.begin(), data.end(), node);
    return it != data.end() ? std::distance(data.begin(), it) : -1;
}

ANode* Binary::objOfIndex(unsigned int index)
{
    return (index < data.size()) ? data[index] : nullptr;
}

int Binary::getLength()
{
    return data.size();
}

bool Binary::compareTwoNodes(ANode* node1, ANode* node2)
{
    return node1->f < node2->f;
}

const char* Binary::toString()
{
    std::string result;

    int len = data.size();
    for (int i = 0; i < len; i++)
    {
        ANode* node = data[i];
        result += cocos2d::StringUtils::format("[%03d,%03d]F=%.03f,G=%.03f,H=%.03f\n", node->x, node->y, node->f, node->g, node->h);
    }

    return result.c_str();
}