#pragma once
class IAsyncProcessor
{
public:
	virtual ~IAsyncProcessor() {}
	virtual void Start() = 0;
	virtual void Stop() = 0;
protected:
	virtual void Run() = 0;
};

