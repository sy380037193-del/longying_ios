#pragma once
#include <vector>
#include <sstream>
#include "cocos2d.h"
#include "ANode.h"

template <typename T>
class AVector : public std::vector<T> {
public:
    AVector() {}
    void init();
};