#include "stdafx.h"
#include "BlocksDeque.h"
#include "typedefs.h"

BlocksDeque::BlocksDeque()
{
}


BlocksDeque::~BlocksDeque()
{
}

shared_ptr<vector<uint8_t>> BlocksDeque::GetBlock(int index)
{
	lock_guard<mutex> lock(blocksMutex);
	int i = 0;
	for (auto it = blocksPtrs.begin(); it != blocksPtrs.end(); it++)
	{
		if (i == index)
			return *it;
		i++;
	}
	throw std::out_of_range("Out of blocks range");
}

void BlocksDeque::PushBackBlock(shared_ptr<vector<uint8_t>> block)
{
	lock_guard<mutex> lock(blocksMutex);
	lock_guard<mutex> lock2(crcsMutex);
	this->blocksPtrs.insert(block);
	this->crcsPtrs.insert(pair<BlockPtr, CrcsPtr>(
												block, 
												CrcsPtr(new vector<uint32_t>())
												)
						);
	//this->crcsPtrs.insert(   shared_ptr<vector<uint32_t>>(new vector<uint32_t>()));
}

void BlocksDeque::PushCrcForBlock(uint32_t crc, shared_ptr<vector<uint8_t>> block)
{
	lock_guard<mutex> lock2(crcsMutex);
	this->crcsPtrs[block]->push_back(crc);
}


void BlocksDeque::DeleteBlock(shared_ptr<vector<uint8_t>> block)
{
	lock_guard<mutex> lock(blocksMutex);
	lock_guard<mutex> lock2(crcsMutex);
	
	blocksPtrs.erase(block);

	for (auto it = crcsPtrs.begin(); it != crcsPtrs.end(); it++)
	{
		if (it->first == block)
		{
			crcsPtrs.erase(it);
			break;
		}
	}
}

int BlocksDeque::GetBlocksCount()
{
	lock_guard<mutex> lock(blocksMutex);
	return blocksPtrs.size();
}


CheckCrcResult BlocksDeque::CheckCrcsForBlock(shared_ptr<vector<uint8_t>> block, int consumersCount)
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
