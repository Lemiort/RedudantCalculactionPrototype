#include "stdafx.h"
#include "BlocksConsumer.h"
#include <thread>

/* CRC-32C (iSCSI) polynomial in reversed bit order. */
#define POLY 0x82f63b78

/* CRC-32 (Ethernet, ZIP, etc.) polynomial in reversed bit order. */
/* #define POLY 0xedb88320 */

BlocksConsumer::BlocksConsumer(BlocksPoolPtr blocksDeque, BlocksCrcsPtr blocksCrcs)
{
	this->blocksDeque = blocksDeque;
	this->blocksCrcs = blocksCrcs;
}


BlocksConsumer::~BlocksConsumer()
{
}

void BlocksConsumer::Start()
{
	doRun = true;
	std::thread runThread = std::thread(&BlocksConsumer::Run, this);
	runThread.detach();
}

void BlocksConsumer::Stop()
{
	doRun = false;
}

void BlocksConsumer::Run()
{
	while (doRun)
	{
		auto it = this->blocksDeque->begin();
		//try to find in crcs
		for (; it != this->blocksDeque->end(); it++)
		{
			if (this->blocksCrcs->count(*it) == 0)
				break;
		}
		//first not calculated block  was found
		if (it != this->blocksDeque->end())
		{
			auto crc = this->Crc32(*it);
			this->blocksCrcs->insert(std::pair<BlockPtr, uint32_t>(*it, crc));
		}
	}
}

uint32_t BlocksConsumer::Crc32(BlockPtr block)
{
	uint32_t crc = 0;
	int k;
	size_t len = block->size();
	auto it = block->begin();

	crc = ~crc;
	while (len--) {
		crc ^= *it++;
		for (k = 0; k < 8; k++)
			crc = crc & 1 ? (crc >> 1) ^ POLY : crc >> 1;
	}
	return ~crc;
}
