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

#include "SignalRModule.h"
#include "Modules/ModuleManager.h"
#include "IHubConnection.h"
#include "WebSocketsModule.h"
#include "SignalRSubsystem.h"
#include "Engine/Engine.h"

IMPLEMENT_MODULE(FSignalRModule, SignalR);

DEFINE_LOG_CATEGORY(LogSignalR);

FSignalRModule* FSignalRModule::Singleton = nullptr;

FSignalRModule& FSignalRModule::Get()
{
    if (nullptr == Singleton)
    {
        check(IsInGameThread());
        FModuleManager::LoadModuleChecked<FSignalRModule>("SignalR");
    }
    check(Singleton);
    return *Singleton;
}

TSharedPtr<IHubConnection> FSignalRModule::CreateHubConnection(const FString& InUrl, const TMap<FString, FString>& InHeaders)
{
    check(bInitialized);
    return GEngine->GetEngineSubsystem<USignalRSubsystem>()->CreateHubConnection(InUrl, InHeaders);
}

void FSignalRModule::StartupModule()
{
    Singleton = this;
    bInitialized = true;
    FModuleManager::LoadModuleChecked<FWebSocketsModule>("WebSockets");
}

void FSignalRModule::ShutdownModule()
{
    bInitialized = false;
}
