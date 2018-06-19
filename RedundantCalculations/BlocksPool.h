#pragma once
#include <vector>
#include <deque>
#include <mutex>
#include <set>
#include <map>
#include "typedefs.h"

using namespace std;
enum CheckCrcResult
{
	NotEnoughCrcs,
	Match,
	DontMatch
};

class BlocksPool
{
public:
	BlocksPool(int maxBlocksCount);
	virtual ~BlocksPool();
	shared_ptr<vector<uint8_t>> GetBlock(int index);
	void PushBackBlock(shared_ptr<vector<uint8_t>> block);
	void PushCrcForBlock(uint32_t crc, shared_ptr<vector<uint8_t>> block);
	void DeleteBlock(shared_ptr<vector<uint8_t>> block);
	int GetBlocksCount();
	int GetBlocksGlobalCounter();
	CheckCrcResult CheckCrcsForBlock(shared_ptr<vector<uint8_t>> block,int consumersCount);
private:
	deque<BlockPtr> blocksPtrs;
	map<BlockPtr, CrcsPtr> crcsPtrs;
	mutex blocksMutex;
	mutex crcsMutex;
	int blocksGlobalCounter = 0;
	int maxBlocksCount = 0;
};

