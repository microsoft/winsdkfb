#include "pch.h"
// Get active session
FBSession^ sess = FBSession::ActiveSession;
if (sess->LoggedIn)
{
    // Set parameters
    PropertySet^ parameters = ref new PropertySet();
    // Set message
    parameters->Insert(L"message", L"Try this sample.");
    // Display requests dialog
    create_task(sess->ShowRequestsDialog(parameters)).then([=](FBResult^ result)
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
