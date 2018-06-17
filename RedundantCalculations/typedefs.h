#pragma once
#include <memory>
#include <deque>
#include <vector>
#include <map>
#include <mutex>
#include "Semaphore.h"

typedef std::shared_ptr<std::vector<uint8_t>> BlockPtr;
typedef std::shared_ptr<std::vector<uint32_t>> CrcsPtr;
typedef std::shared_ptr<std::deque<BlockPtr>> BlocksPoolPtr;
typedef std::map<BlockPtr, uint32_t> BlocksCrcs;
typedef std::shared_ptr<BlocksCrcs> BlocksCrcsPtr;
typedef std::shared_ptr<std::mutex> MutexPtr;
typedef std::shared_ptr<Semaphore> SemaphorePtr;

