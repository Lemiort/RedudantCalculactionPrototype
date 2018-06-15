#pragma once
#include "typedefs.h"
#include <mutex>
#include <atomic>
#include "IAsyncProcessor.h"



class BlocksProducer:public IAsyncProcessor
{
public:
	BlocksProducer(BlocksPoolPtr blocksDeque, MutexPtr dequeLock);
	virtual void Start();
	virtual void Stop();
	virtual ~BlocksProducer();

	int blockSize = 512;
	int blocksCount = 512 * 10;
private:
	//infinite loop
	virtual void Run();
	BlockPtr GenerateBlock();
	
	MutexPtr dequeLock;
	BlocksPoolPtr blocksDeque;


	//do Run() while this flag true
	std::atomic_bool doRun = false;
};

