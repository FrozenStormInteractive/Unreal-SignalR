<h1 align="center">
  <br>
  <a href="https://github.com/FrozenStormInteractive/Unreal-SignalR">
    <img src="Resources/Icon128.png" alt="Logo" width="120">
   </a>
  <br>
  SignalR client for Unreal Engine
  <br>
</h1>

## Dependencies

This plug-in requires Visual Studio and either a C++ code project or a full Unreal Engine source code from GitHub.

**This plugin does not support the MessagePack protocol for the moment, only the JSON protocol.**

## Usage

You can use this plug-in as a project plug-in, or an Engine plug-in:

 - If you use it as a project plug-in, clone this repository into your project's `Plugins` directory and compile your game in Visual Studio. A C++ code project is required for this to work.

 - If you use it as an Engine plug-in, clone this repository into the `Engine/Plugins` directory and compile your game. *Full Unreal Engine source code from GitHub is required for this*.

This plug-in is enabled by default, so no need to enable it in the plug-in browser.

Link the `SignalR` module to to yours with `PublicDependencyModuleNames` or `PrivateDependencyModuleNames` in `<YourModule>.build.cs`:
```cs
PrivateDependencyModuleNames.AddRange(new string[]
{
    "SignalR",
}
);
```

Create a hub connection with the SignalR engine subsystem:
```cpp
#include "SignalRModule.h"
#include "IHubConnection.h"

TSharedPtr<IHubConnection> Hub = GEngine->GetEngineSubsystem<USignalRSubsystem>()->CreateHubConnection("https://example.com/chathub");
```

Bind an event which is fired when the server call it to the client.
```cpp
Hub->On(TEXT("EventName")).BindLambda([](const TArray<FSignalRValue>& Arguments) {
    ...
});
```

`Invoke` fires an event when the server has finished invoking the method (or an error occurred). In addition, the event
can receive a result from the server method, if the server returns a result.
```cpp
Hub->Invoke(TEXT("Add"), 1, 1).BindLambda([](const FSignalRValue& Result) {
    UE_LOG(LogTemp, Warning, TEXT("The result value is: %d"), Result.AsInt());
});
```

Unlike the `Invoke` method, the `Send` method doesn't wait for a response from the server.
```cpp
Hub->Send(TEXT("Add"), 1, 1);
```

## Contributing

Please see [CONTRIBUTING.md](CONTRIBUTING.md) for instructions on how to contribute.

## License

This project is licensed under the MIT License - see the [LICENSE.txt](LICENSE.txt) file for details.

```
Copyright (c) 2020-2021 FrozenStorm Interactive, Yoann Potinet
```
