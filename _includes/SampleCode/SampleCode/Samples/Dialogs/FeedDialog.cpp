#include "pch.h"
#include "FBReturnObject.h"

using namespace Windows::Foundation::Collections;
using namespace Platform;
using namespace concurrency;
using namespace winsdkfb;
using namespace winsdkfb::Graph;

namespace SampleCode
{
    void ShowFeedDialog()
    {
        // Get active session
        FBSession^ sess = FBSession::ActiveSession;
        if (sess->LoggedIn)
        {
            // Set caption, link and description parameters
            PropertySet^ parameters = ref new PropertySet();
            parameters->Insert(L"caption", L"Microsoft");
            parameters->Insert(L"link", L"https://www.microsoft.com/en-us/default.aspx");
            parameters->Insert(L"description", L"Microsoft home page");
            // Display feed dialog
            create_task(sess->ShowFeedDialogAsync(parameters)).then([=](FBResult^ result)
            {
                if (result->Succeeded)
                {
                    // Posting succeeded
                }
                else
                {
                    // Posting failed
                }
            });
        }
    }
}
