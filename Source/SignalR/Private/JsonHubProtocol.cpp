// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

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
        // number
        Out = TEXT("null");
        break;
    }
    case EJson::Boolean:
    {
        // number
        Out = InValue->AsBool() ? TEXT("true") : TEXT("false");
        break;
    }
    case EJson::Number:
    {
        // number
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
            Out.Empty();
            break;
        }
    }
    default:
    {
        UE_LOG(LogSignalR, Error, TEXT("Cannot convert JSON value to string."));
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
