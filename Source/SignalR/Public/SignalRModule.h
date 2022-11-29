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
