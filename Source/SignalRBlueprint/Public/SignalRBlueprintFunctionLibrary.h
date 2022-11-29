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
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SignalRHubConnectionWrapper.h"
#include "SignalRValueWrapper.h"
#include "SignalRBlueprintFunctionLibrary.generated.h"

UENUM(BlueprintType)
enum class ESignalRValueCastResult : uint8
{
    Success,
    Failed,
};

UCLASS()
class SIGNALRBLUEPRINT_API USignalRBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintPure, Category ="SignalR", Meta = (DisplayName = "SignalR Make Int"))
    static FSignalRValueWrapper MakeInt(int Value);

    UFUNCTION(BlueprintPure, Category = "SignalR", Meta = (DisplayName = "SignalR Make Float"))
    static FSignalRValueWrapper MakeFloat(float Value);

    UFUNCTION(BlueprintPure, Category = "SignalR", Meta = (DisplayName = "SignalR Make Number"))
    static FSignalRValueWrapper MakeNumber(double Value);

    UFUNCTION(BlueprintPure, Category = "SignalR", Meta = (DisplayName = "SignalR Make String"))
    static FSignalRValueWrapper MakeString(const FString& Value);

    UFUNCTION(BlueprintPure, Category = "SignalR", Meta = (DisplayName = "SignalR Make Bool"))
    static FSignalRValueWrapper MakeBool(bool Value);

    UFUNCTION(BlueprintCallable, Category = "SignalR", Meta = (ExpandEnumAsExecs = "CastResult"))
    static int64 AsInt(const FSignalRValueWrapper& SignalRValue, ESignalRValueCastResult& CastResult);

    UFUNCTION(BlueprintCallable, Category = "SignalR", Meta = (ExpandEnumAsExecs = "CastResult"))
    static float AsFloat(const FSignalRValueWrapper& SignalRValue, ESignalRValueCastResult& CastResult);

    UFUNCTION(BlueprintCallable, Category = "SignalR", Meta = (ExpandEnumAsExecs = "CastResult"))
    static double AsNumber(const FSignalRValueWrapper& SignalRValue, ESignalRValueCastResult& CastResult);

    UFUNCTION(BlueprintCallable, Category = "SignalR", Meta = (ExpandEnumAsExecs = "CastResult"))
    static FString AsString(const FSignalRValueWrapper& SignalRValue, ESignalRValueCastResult& CastResult);

    UFUNCTION(BlueprintCallable, Category = "SignalR", Meta = (ExpandEnumAsExecs = "CastResult"))
    static bool AsBool(const FSignalRValueWrapper& SignalRValue, ESignalRValueCastResult& CastResult);
};
