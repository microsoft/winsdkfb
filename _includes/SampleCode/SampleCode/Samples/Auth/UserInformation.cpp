#include "pch.h"
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
