#include "pch.h"

using namespace Windows::Foundation::Collections;
using namespace Platform;
using namespace concurrency;
using namespace winsdkfb;
using namespace winsdkfb::Graph;

namespace SampleCode
{
    void Logout()
    {
        FBSession^ sess = FBSession::ActiveSession;
        sess->LogoutAsync();
    }
}
