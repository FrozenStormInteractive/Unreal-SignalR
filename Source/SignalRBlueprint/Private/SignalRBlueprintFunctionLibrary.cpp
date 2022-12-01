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

#include "SignalRBlueprintFunctionLibrary.h"

FSignalRValueWrapper USignalRBlueprintFunctionLibrary::MakeInt(int Value)
{
    return FSignalRValue(Value);
}

FSignalRValueWrapper USignalRBlueprintFunctionLibrary::MakeFloat(float Value)
{
    return FSignalRValue(Value);
}

FSignalRValueWrapper USignalRBlueprintFunctionLibrary::MakeNumber(double Value)
{
    return FSignalRValue(Value);
}

FSignalRValueWrapper USignalRBlueprintFunctionLibrary::MakeString(const FString& Value)
{
    return FSignalRValue(Value);
}

FSignalRValueWrapper USignalRBlueprintFunctionLibrary::MakeBool(bool Value)
{
    return FSignalRValue(Value);
}

int64 USignalRBlueprintFunctionLibrary::AsInt(const FSignalRValueWrapper& SignalRValue, ESignalRValueCastResult& CastResult)
{
    if (SignalRValue.InternalValue.IsNumber())
    {
        CastResult = ESignalRValueCastResult::Success;
        return SignalRValue.InternalValue.AsInt();
    }
    else
    {
        CastResult = ESignalRValueCastResult::Failed;
        return 0;
    }
}

float USignalRBlueprintFunctionLibrary::AsFloat(const FSignalRValueWrapper& SignalRValue, ESignalRValueCastResult& CastResult)
{
    if (SignalRValue.InternalValue.IsNumber())
    {
        CastResult = ESignalRValueCastResult::Success;
        return SignalRValue.InternalValue.AsFloat();
    }
    else
    {
        CastResult = ESignalRValueCastResult::Failed;
        return 0.0f;
    }
}

double USignalRBlueprintFunctionLibrary::AsNumber(const FSignalRValueWrapper& SignalRValue, ESignalRValueCastResult& CastResult)
{
    if (SignalRValue.InternalValue.IsNumber())
    {
        CastResult = ESignalRValueCastResult::Success;
        return SignalRValue.InternalValue.AsNumber();
    }
    else
    {
        CastResult = ESignalRValueCastResult::Failed;
        return 0.0;
    }
}

FString USignalRBlueprintFunctionLibrary::AsString(const FSignalRValueWrapper& SignalRValue, ESignalRValueCastResult& CastResult)
{
    if (SignalRValue.InternalValue.IsString())
    {
        CastResult = ESignalRValueCastResult::Success;
        return SignalRValue.InternalValue.AsString();
    }
    else
    {
        CastResult = ESignalRValueCastResult::Failed;
        return TEXT("");
    }
}

bool USignalRBlueprintFunctionLibrary::AsBool(const FSignalRValueWrapper& SignalRValue, ESignalRValueCastResult& CastResult)
{
    if (SignalRValue.InternalValue.IsBoolean())
    {
        CastResult = ESignalRValueCastResult::Success;
        return SignalRValue.InternalValue.AsBool();
    }
    else
    {
        CastResult = ESignalRValueCastResult::Failed;
        return false;
    }
}

bool USignalRBlueprintFunctionLibrary::HasError(const FSignalRInvokeResultWrapper& Result)
{
    return Result.InternalResult.HasError();
}

FString USignalRBlueprintFunctionLibrary::GetSignalRInvokeResultErrorMessage(const FSignalRInvokeResultWrapper& Result)
{
    return Result.InternalResult.GetErrorMessage();
}

FSignalRValueWrapper USignalRBlueprintFunctionLibrary::GetSignalRInvokeResultValue(const FSignalRInvokeResultWrapper& Result)
{
    return Result.InternalValue;
}
