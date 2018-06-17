#pragma once
#include "typedefs.h"
#include <stddef.h>
#include <stdint.h>
#include "IAsyncProcessor.h"
#include <atomic>
#include "Semaphore.h"


class BlocksConsumer: public IAsyncProcessor
{
public:
	BlocksConsumer(BlocksPoolPtr blocksDeque, MutexPtr dequeWriteFlag, SemaphorePtr readSemaphore, BlocksCrcsPtr blocksCrcs, MutexPtr crcsLock);
	~BlocksConsumer();
	virtual void Start();
	virtual void Stop();
	BlocksPoolPtr blocksDeque;
	BlocksCrcsPtr blocksCrcs;
private:
	virtual void Run();
	uint32_t Crc32(BlockPtr block);
	MutexPtr blocksDequeLock;
	MutexPtr crcsLock;
	SemaphorePtr dequeReadSemaphore;

	//do Run() while this flag true
	std::atomic_bool doRun = false;
};

