#pragma once

#include "CoreMinimal.h"

enum class ESignalRMessageType : uint8
{
	Invocation = 1,
	StreamItem = 2,
	Completion = 3,
	StreamInvocation = 4,
	CancelInvocation = 5,
	Ping = 6,
	Close = 7,
};
