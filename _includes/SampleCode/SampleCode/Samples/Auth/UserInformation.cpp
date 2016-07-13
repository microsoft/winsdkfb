#include "pch.h"

using namespace Windows::Foundation::Collections;
using namespace Platform;
using namespace concurrency;
using namespace winsdkfb;
using namespace winsdkfb::Graph;

namespace SampleCode
{
    void UserInformation()
    {
        // Get active session
        FBSession^ sess = FBSession::ActiveSession;
        if (sess->LoggedIn)
        {
            FBUser^ user = sess->User;
            if (user)
            {
                String^ userId = L"Id : " + user->Id;
                String^ username = L"Name : " + user->Name;
                String^ locale = L"Locale : " + user->Locale;
            }
        }
    }
}
