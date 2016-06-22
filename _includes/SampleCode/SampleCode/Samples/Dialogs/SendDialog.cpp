#include "pch.h"
#include "FBReturnObject.h"

using namespace Windows::Foundation::Collections;
using namespace Platform;
using namespace concurrency;
using namespace winsdkfb;
using namespace winsdkfb::Graph;

namespace SampleCode
{
    void ShowSendDialog()
    {
        //Get active session
        FBSession^ sess = FBSession::ActiveSession;
        if (sess->LoggedIn)
        {
            PropertySet^ parameters = ref new PropertySet();
            parameters->Insert(L"link", L"https://www.microsoft.com/en-us/default.aspx");
            //Display send dialog
            create_task(sess->ShowSendDialogAsync(parameters)).then([=](FBResult^ result)
            {
                if (result->Succeeded)
                {
                    //message successfully sent
                }
                else
                {
                    //message failed to send
                }
            });
        }
    }
}
