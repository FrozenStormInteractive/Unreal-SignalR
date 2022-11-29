/*
 * MIT License
 *
 * Copyright (c) 2020-2021 Frozen Storm Interactive
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include "CoreMinimal.h"
#include "SignalRValue.h"
#include "SignalRValueWrapper.generated.h"

USTRUCT(BlueprintType, DisplayName = "SignalR Value")
struct FSignalRValueWrapper
{
    GENERATED_BODY()

    FSignalRValueWrapper() = default;
    FSignalRValueWrapper(const FSignalRValue& Value):
        InternalValue(Value)
    {}

    explicit FSignalRValueWrapper(const FSignalRValueWrapper& Other);
    FSignalRValueWrapper(FSignalRValueWrapper&& Other) noexcept;

    FSignalRValueWrapper& operator=(FSignalRValueWrapper const& Other);
    FSignalRValueWrapper& operator=(FSignalRValueWrapper&& Other) noexcept;

    FSignalRValue InternalValue;
};

#if 0
USTRUCT(BlueprintType, DisplayName = "SignalR Invoke Result")
struct FSignalRInvokeResultWrapper
{
    GENERATED_BODY()
};
#endif
