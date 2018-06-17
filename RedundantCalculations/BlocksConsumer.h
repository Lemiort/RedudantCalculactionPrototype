#pragma once
#include "typedefs.h"
#include <stddef.h>
#include <stdint.h>
#include "IAsyncProcessor.h"
#include <atomic>
#include "BlocksDeque.h"
#include <set>

class BlocksConsumer: public IAsyncProcessor
{
public:
	BlocksConsumer(std::shared_ptr<BlocksDeque> blocksDeque);
	~BlocksConsumer();
	virtual void Start();
	virtual void Stop();
private:
	virtual void Run();
	uint32_t Crc32(BlockPtr block);
	std::shared_ptr<BlocksDeque> blocksDeque;
	std::set<BlockPtr> processedBlocks;

	//do Run() while this flag true
	std::atomic_bool doRun = false;
};

