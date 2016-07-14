#include "pch.h"

using namespace Windows::Foundation::Collections;
using namespace Platform;
using namespace concurrency;
using namespace winsdkfb;
using namespace winsdkfb::Graph;

namespace SampleCode
{
    void Initialization()
    {
        FBSession^ sess = FBSession::ActiveSession;
        sess->FBAppId = "<Facebook App ID>";
        sess->WinAppId = "<Windows or Windows Phone Store ID depending on the target device";
    }
}
