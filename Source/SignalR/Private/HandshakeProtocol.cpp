// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "HandshakeProtocol.h"
#include "IHubProtocol.h"
#include "JsonHubProtocol.h"
#include "JsonSerializer.h"
#include "SignalRModule.h"

FString FHandshakeProtocol::CreateHandshakeMessage(TSharedPtr<IHubProtocol> InProtocol)
{
    TMap<FString, TSharedPtr<FJsonValue>> Values
    {
        { "protocol", MakeShared<FJsonValueString>(InProtocol->Name().ToString()) },
        { "version", MakeShared<FJsonValueNumber>(InProtocol->Version()) },
    };
    TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
    Obj->Values = Values;

    FString Out;
    TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&Out);
    FJsonSerializer::Serialize(Obj.ToSharedRef(), JsonWriter);

    return Out + FJsonHubProtocol::RecordSeparator;
}

TTuple<TSharedPtr<FJsonObject>, FString> FHandshakeProtocol::ParseHandshakeResponse(const FString& Response)
{
    int32 Pos = -1;
    if(!Response.FindChar(FJsonHubProtocol::RecordSeparator, Pos))
    {
        return MakeTuple<TSharedPtr<FJsonObject>, FString>(nullptr, Response.Mid(0));
    }

    FString HandshakeMessage = Response.Mid(0, Pos);

    TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();

    TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(HandshakeMessage);
    if (!FJsonSerializer::Deserialize(JsonReader, JsonObject))
    {
        UE_LOG(LogSignalR, Warning, TEXT("Cannot unserialize handshake message: %s"), *JsonReader->GetErrorMessage());
        JsonObject = nullptr;
    }

    FString RemainingData = Response.Mid(Pos + 1);

    return MakeTuple(JsonObject, RemainingData);
}
