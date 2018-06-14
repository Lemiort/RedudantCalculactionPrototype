#pragma once
#include <memory>
#include <deque>
#include <vector>

typedef std::shared_ptr<std::vector<uint8_t>> BlockPtr;
typedef std::shared_ptr<std::deque<BlockPtr>> BlocksPool;

