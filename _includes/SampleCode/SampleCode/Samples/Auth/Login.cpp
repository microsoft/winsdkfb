#include "pch.h"
// Get active session
FBSession^ sess = FBSession::ActiveSession;

// Add permissions required by the app
Vector<String^>^ permissionList = ref new Vector<String^>(); ("public_profile");
permissionList->Append(L"public_profile");
permissionList->Append(L"user_friends");
permissionList->Append(L"user_likes");
permissionList->Append(L"user_groups");
permissionList->Append(L"user_location");
permissionList->Append(L"user_photos");
permissionList->Append(L"publish_actions");
FBPermissions^ permissions = ref new FBPermissions(permissionList->GetView());

// Login to Facebook
create_task(sess->LoginAsync(permissions)).then([=](FBResult^ result)
{
    if (result->Succeeded)
    {
        // Login succeeded
        ...
    }
    else
    {
        // Login failed
        ...
    }
});
