#include "stdafx.h"
#include "BlocksPool.h"
#include "typedefs.h"

BlocksPool::BlocksPool(int maxBlocksCount = 0)
{
	this->maxBlocksCount = maxBlocksCount;
}


BlocksPool::~BlocksPool()
{
}

shared_ptr<vector<uint8_t>> BlocksPool::GetBlock(int index)
{
	lock_guard<mutex> lock(blocksMutex);
	/*int i = 0;
	for (auto it = blocksPtrs.begin(); it != blocksPtrs.end(); it++)
	{
		if (i == index)
			return *it;
		i++;
	}
	throw std::out_of_range("Out of blocks range");*/
	return blocksPtrs.at(index);
}

void BlocksPool::PushBackBlock(shared_ptr<vector<uint8_t>> block)
{
	lock_guard<mutex> lock(blocksMutex);
	lock_guard<mutex> lock2(crcsMutex);
	if (maxBlocksCount != 0 && maxBlocksCount <= this->blocksPtrs.size())
		throw std::out_of_range("Trying to push more blocks then BlocksDeque can store");

	this->blocksPtrs.push_back(block);
	this->crcsPtrs.insert(pair<BlockPtr, CrcsPtr>(
												block, 
												CrcsPtr(new vector<uint32_t>())
												)
						);
	blocksGlobalCounter++;
	//this->crcsPtrs.insert(   shared_ptr<vector<uint32_t>>(new vector<uint32_t>()));
}

void BlocksPool::PushCrcForBlock(uint32_t crc, shared_ptr<vector<uint8_t>> block)
{
	lock_guard<mutex> lock2(crcsMutex);
	this->crcsPtrs[block]->push_back(crc);
}


void BlocksPool::DeleteBlock(shared_ptr<vector<uint8_t>> block)
{
	lock_guard<mutex> lock(blocksMutex);
	lock_guard<mutex> lock2(crcsMutex);
	
	//blocksPtrs.erase(block);
	for (auto it = blocksPtrs.begin(); it != blocksPtrs.end(); it++)
	{
		if (*it == block)
		{
			blocksPtrs.erase(it);
			break;
		}
	}

	

	for (auto it = crcsPtrs.begin(); it != crcsPtrs.end(); it++)
	{
		if (it->first == block)
		{
			crcsPtrs.erase(it);
			break;
		}
	}
}

int BlocksPool::GetBlocksCount()
{
	lock_guard<mutex> lock(blocksMutex);
	return blocksPtrs.size();
}

int BlocksPool::GetBlocksGlobalCounter()
{
	return blocksGlobalCounter;
}


CheckCrcResult BlocksPool::CheckCrcsForBlock(shared_ptr<vector<uint8_t>> block, int consumersCount)
{
	int i = 0;
	//pointer to vector
	auto crcs = this->crcsPtrs[block];

	if ( crcs->size() < consumersCount)
	{
		return CheckCrcResult::NotEnoughCrcs;
	}

	uint32_t crc = (*crcs)[0];
	for (auto it = crcs->begin(); it != crcs->end(); it++)
	{
		if (*it != crc)
		{
			return CheckCrcResult::DontMatch;
		}
		crc = *it;
	}
	return CheckCrcResult::Match;
}
