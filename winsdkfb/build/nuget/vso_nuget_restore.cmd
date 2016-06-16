
cd %~dp0\..\..
.\build\nuget\Nuget.exe restore winsdkfb.sln -NonInteractive
.\build\nuget\nuget.exe restore winsdkfb_uwp\winsdkfb_uwp.sln -NonInteractive
