#pragma once

#include "Runtime/Engine/Public/Tickable.h"

class FTickProxy : public FTickableGameObject
{
private:
	// Tick process
	TFunction<void()> Work;

public:
	// Constructor
	FTickProxy(const TFunction<void()> InWork) : Work(InWork) {}

	//TickEvent
	virtual TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(FTickProxy, STATGROUP_Tickables); }
	virtual bool IsTickable() const { return true; }
	virtual void Tick(float DeltaTime) { Work(); }
};
