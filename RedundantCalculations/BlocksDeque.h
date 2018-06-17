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

class BlocksDeque
{
public:
	BlocksDeque();
	virtual ~BlocksDeque();
	shared_ptr<vector<uint8_t>> GetBlock(int index);
	void PushBackBlock(shared_ptr<vector<uint8_t>> block);
	void PushCrcForBlock(uint32_t crc, shared_ptr<vector<uint8_t>> block);
	void DeleteBlock(shared_ptr<vector<uint8_t>> block);
	int GetBlocksCount();
	CheckCrcResult CheckCrcsForBlock(shared_ptr<vector<uint8_t>> block,int consumersCount);
private:
	set<BlockPtr> blocksPtrs;
	map<BlockPtr, CrcsPtr> crcsPtrs;
	mutex blocksMutex;
	mutex crcsMutex;
};

