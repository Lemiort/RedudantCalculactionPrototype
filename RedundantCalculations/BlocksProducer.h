#pragma once
#include "typedefs.h"
#include <mutex>
#include <atomic>



class BlocksProducer
{
public:
	BlocksProducer(BlocksPool blocksDeque);
	void Start();
	void Stop();
	virtual ~BlocksProducer();

	BlocksPool blocksDeque;
	int blockSize = 512;
	int blocksCount = 512*10;
private:
	//infinite loop
	void Run();
	BlockPtr GenerateBlock();
	
	//std::mutex dequeLock;

	//do Run() while this flag true
	std::atomic_bool doRun = false;
};

