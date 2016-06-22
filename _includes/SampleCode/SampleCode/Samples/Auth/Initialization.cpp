#include "pch.h"
using namespace winsdkfb;
...
FBSession^ sess = FBSession::ActiveSession;
sess->FBAppId = "<Facebook App ID>";
sess->WinAppId = "<Windows or Windows Phone Store ID depending on the target device";
