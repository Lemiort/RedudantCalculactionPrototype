#pragma once
#include "typedefs.h"
#include <mutex>
#include <atomic>
#include "IAsyncProcessor.h"
#include "BlocksDeque.h"


class BlocksProducer:public IAsyncProcessor
{
public:
	BlocksProducer(std::shared_ptr<BlocksDeque> blocksDeque);
	virtual void Start();
	virtual void Stop();
	virtual ~BlocksProducer();

	int blockSize = 512;
	int blocksCount = 512 * 10;
private:
	//infinite loop
	virtual void Run();
	BlockPtr GenerateBlock();
	std::shared_ptr<BlocksDeque> blocksDeque;


	//do Run() while this flag true
	std::atomic_bool doRun = false;
};

