#include "pch.h"
//Get active session
FBSession^ sess = FBSession::ActiveSession;
if (sess->LoggedIn)
{
    PropertySet^ parameters = ref new PropertySet();
    parameters->Insert(L"link", L"https://www.microsoft.com/en-us/default.aspx");
    //Display send dialog
    create_task(sess->ShowSendDialog(parameters)).then([=](FBResult^ result))
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
