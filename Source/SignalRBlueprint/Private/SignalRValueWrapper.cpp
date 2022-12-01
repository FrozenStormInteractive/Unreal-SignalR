/*
 * MIT License
 *
 * Copyright (c) 2020-2022 Frozen Storm Interactive, Yoann Potinet
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

#include "SignalRValueWrapper.h"

FSignalRValueWrapper::FSignalRValueWrapper(const FSignalRValue& Value):
    InternalValue(Value)
{}

FSignalRValueWrapper::FSignalRValueWrapper(const FSignalRValueWrapper& Other):
    InternalValue(Other.InternalValue)
{
}

FSignalRValueWrapper::FSignalRValueWrapper(FSignalRValueWrapper&& Other) noexcept:
    InternalValue(MoveTemp(Other.InternalValue))
{
}

FSignalRValueWrapper& FSignalRValueWrapper::operator=(FSignalRValueWrapper const& Other)
{
    InternalValue = Other.InternalValue;
    return *this;
}

FSignalRValueWrapper& FSignalRValueWrapper::operator=(FSignalRValueWrapper&& Other) noexcept
{
    InternalValue = MoveTemp(Other.InternalValue);
    return *this;
}

FSignalRInvokeResultWrapper::FSignalRInvokeResultWrapper(const FSignalRInvokeResult& Value): FSignalRValueWrapper(Value),
    InternalResult(Value)
{
}

FSignalRInvokeResultWrapper::FSignalRInvokeResultWrapper(const FSignalRInvokeResultWrapper& Other): FSignalRValueWrapper(Other),
    InternalResult(Other.InternalValue)
{
}

FSignalRInvokeResultWrapper::FSignalRInvokeResultWrapper(FSignalRInvokeResultWrapper&& Other) noexcept: FSignalRValueWrapper(Other),
    InternalResult(MoveTemp(Other.InternalValue))
{
}

FSignalRInvokeResultWrapper& FSignalRInvokeResultWrapper::operator=(FSignalRInvokeResultWrapper const& Other)
{
    InternalValue = Other.InternalValue;
    InternalResult = Other.InternalResult;
    return *this;
}

FSignalRInvokeResultWrapper& FSignalRInvokeResultWrapper::operator=(FSignalRInvokeResultWrapper&& Other) noexcept
{
    InternalValue = MoveTemp(Other.InternalValue);
    InternalResult = MoveTemp(Other.InternalResult);
    return *this;
}
