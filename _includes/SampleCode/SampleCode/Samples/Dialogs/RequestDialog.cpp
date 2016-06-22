#include "pch.h"
#include "FBReturnObject.h"

using namespace Windows::Foundation::Collections;
using namespace Platform;
using namespace concurrency;
using namespace winsdkfb;
using namespace winsdkfb::Graph;

namespace SampleCode
{
    void ShowRequestDialog()
    {
        // Get active session
        FBSession^ sess = FBSession::ActiveSession;
        if (sess->LoggedIn)
        {
            // Set parameters
            PropertySet^ parameters = ref new PropertySet();
            // Set message
            parameters->Insert(L"message", L"Try this sample.");
            // Display requests dialog
            create_task(sess->ShowRequestsDialogAsync(parameters)).then([=](FBResult^ result)
            {
                if (result->Succeeded)
                {
                    // Requests sent
                }
                else
                {
                    // Sending requests failed
                }
            });
        }
    }
}
