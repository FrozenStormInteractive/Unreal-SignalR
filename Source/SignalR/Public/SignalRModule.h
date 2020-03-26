// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSignalR, Log, All);

class IHubConnection;

class FSignalRModule : public IModuleInterface
{
public:
    /**
     * Singleton-like access to this module's interface.
     * Beware of calling this during the shutdown phase, though. Your module might have been unloaded already.
     *
     * @return Returns singleton instance, loading the module on demand if needed
     */
    SIGNALR_API static FSignalRModule& Get();

    /**
     *
     * @param url 
     * @return An IHubConnection instance
     */
    SIGNALR_API TSharedPtr<IHubConnection> CreateHubConnection(const FString& InUrl, const TMap<FString, FString>& InHeaders = TMap<FString, FString>());

private:

    virtual void StartupModule() override;

    virtual void ShutdownModule() override;
	
	virtual bool SupportsDynamicReloading() override
	{
		return false;
	}

	virtual bool SupportsAutomaticShutdown() override
	{
		return false;
	}

    /** Singleton Instance */
    static FSignalRModule* Singleton;

    /** Whether this module has been initialized */
    bool bInitialized = false;
};
