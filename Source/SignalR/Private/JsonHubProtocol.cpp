/*
 * MIT License
 *
 * Copyright (c) 2020-2021 FrozenStorm Interactive
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

#include "JsonHubProtocol.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "SignalRModule.h"

FName FJsonHubProtocol::Name() const
{
    return "json";
}

int FJsonHubProtocol::Version() const
{
    return 1;
}

FString FJsonHubProtocol::ConvertMessage(TSharedPtr<FSignalRValue> InValue) const
{
    FString Out = "";

    switch (InValue->Type)
    {
    case EJson::Null:
    {
        Out = TEXT("null");
        break;
    }
    case EJson::Boolean:
    {
        Out = InValue->AsBool() ? TEXT("true") : TEXT("false");
        break;
    }
    case EJson::Number:
    {
        Out = FString::SanitizeFloat(InValue->AsNumber());
        break;
    }
    case EJson::String:
    {
        Out = InValue->AsString();
        break;
    }
    case EJson::Object:
    {
        TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&Out);
        if(FJsonSerializer::Serialize(InValue->AsObject().ToSharedRef(), JsonWriter))
        {
            break;
        }
        else
        {
            UE_LOG(LogSignalR, Error, TEXT("Cannot convert JSON object to string"));
        }
    }
    default:
    {
        UE_LOG(LogSignalR, Error, TEXT("Cannot convert JSON value to string"));
        break;
    }
    }

    return Out;
}

TArray<TSharedPtr<FSignalRValue>> FJsonHubProtocol::ParseMessages(const FString& InStr) const
{
    TArray<TSharedPtr<FSignalRValue>> Arr;

    FString TmpStr(InStr);

    int32 Pos = 0;
    while(TmpStr.FindChar(RecordSeparator, Pos))
    {
        FString MessagePayload = TmpStr.Mid(0, Pos);

        TSharedPtr<FJsonValue> JsonValue;
        TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(MessagePayload);
        if (!FJsonSerializer::Deserialize(JsonReader, JsonValue))
        {
            UE_LOG(LogSignalR, Error, TEXT("Cannot unserialize SignalR message: %s: %s"), *JsonReader->GetErrorMessage(), *MessagePayload);
        }
        else if (JsonValue->Type != EJson::Object)
        {
            UE_LOG(LogSignalR, Error, TEXT("Message was not a 'map' type"));
        }
        else
        {
            TSharedPtr<FJsonObject> Obj = JsonValue->AsObject();

            if (!Obj->HasField(TEXT("type")))
            {
                UE_LOG(LogSignalR, Error, TEXT("Field 'type' not found"));
            }
            else
            {
                Arr.Add(JsonValue);
            }
        }

        TmpStr = TmpStr.Mid(Pos + 1);
    }

    return Arr;
}
