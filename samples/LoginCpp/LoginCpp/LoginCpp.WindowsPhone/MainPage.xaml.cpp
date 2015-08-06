//******************************************************************************
//
// Copyright (c) 2015 Microsoft Corporation. All rights reserved.
//
// This code is licensed under the MIT License (MIT).
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
//******************************************************************************

//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include "OptionsPage.xaml.h"

using namespace concurrency;
using namespace Facebook;
using namespace LoginCpp;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Resources;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Security::Authentication::Web;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

#define FBAppIDName L"FBApplicationId"
#define FBStoreAppIDName L"FBWindowsAppId"
#define PermissionGranted L"granted"

const wchar_t* requested_permissions[] =
{
    L"public_profile",
    L"user_friends",
    L"user_likes",
    L"user_groups",
    L"user_location"
};

MainPage::MainPage()
{
    InitializeComponent();

    String^ whatever = WebAuthenticationBroker::GetCurrentApplicationCallbackUri()->DisplayUri + L"\n";
    OutputDebugString(whatever->Data());

    SetSessionAppIds();
}

void MainPage::SetSessionAppIds()
{
    FBSession^ s = FBSession::ActiveSession;

    // Assumes the Facebook App ID and Windows Phone Store ID have been saved
    // in the default resource file.
    ResourceLoader^ rl = ResourceLoader::GetForCurrentView();

    String^ appId = rl->GetString(FBAppIDName);
    String^ winAppId = rl->GetString(FBStoreAppIDName);

    // IDs are both sent to FB app, so it can validate us.
    s->FBAppId = appId;
    s->WinAppId = winAppId;
}

FBPermissions^ MainPage::BuildPermissions(
    )
{
    FBPermissions^ result = ref new FBPermissions();
    Vector<String^>^ v = ref new Vector<String^>();

    for (unsigned int i = 0; i < ARRAYSIZE(requested_permissions); i++)
    {
        v->Append(ref new String(requested_permissions[i]));
    }

    result->Values = v->GetView();

    return result;
}

BOOL MainPage::DidGetAllRequestedPermissions(
    )
{
    BOOL success = FALSE;
    FBAccessTokenData^ data = FBSession::ActiveSession->AccessTokenData;
    unsigned int grantedCount = 0;

    if (data)
    {
        for (unsigned int i = 0; i < ARRAYSIZE(requested_permissions); i++)
        {
            String^ perm = ref new String(requested_permissions[i]);
            if (data->Permissions && (data->Permissions->HasKey(perm)))
            {
                String^ Value = data->Permissions->Lookup(perm);
                if (!String::CompareOrdinal(Value, PermissionGranted))
                {
                    grantedCount++;
                }
            }
        }

        if (grantedCount == ARRAYSIZE(requested_permissions))
        {
            success = TRUE;
        }
    }

    return success;
}

BOOL MainPage::WasAppPermissionRemovedByUser(
    FBResult^ result
    )
{
    return (result &&
        (!result->Succeeded) &&
        ((result->ErrorInfo->Code == (int)Facebook::ErrorCode::ErrorCodeOauthException) &&
            (result->ErrorInfo->Subcode == (int)Facebook::ErrorSubcode::ErrorSubcodeSessionInvalidated)));
}

BOOL MainPage::ShouldRerequest(
    FBResult^ result
    )
{
    return (result &&
        result->Succeeded &&
        !DidGetAllRequestedPermissions());
}

void MainPage::NavigateToOptionsPage()
{
    LoginButton->Content = L"Logout";

    CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
        Windows::UI::Core::CoreDispatcherPriority::Normal,
        ref new Windows::UI::Core::DispatchedHandler([this]()
    {
        LoginCpp::App^ a = dynamic_cast<LoginCpp::App^>(Application::Current);
        Windows::UI::Xaml::Controls::Frame^ f = a->CreateRootFrame();
        f->Navigate(OptionsPage::typeid);
    }));
}

void MainPage::TryRerequest(
    BOOL retry
    )
{
    // If we're logged out, the session has cleared the FB and Windows app IDs,
    // so they need to be set again.  We load these IDs via the ResourceLoader
    // class, which must be accessed on the UI thread, which is why this call
    // is outside the task context.
    SetSessionAppIds();

    create_task(FBSession::ActiveSession->LoginAsync(BuildPermissions()))
        .then([=](FBResult^ result)
    {
        if (result->Succeeded)
        {
            if (retry && (!DidGetAllRequestedPermissions()))
            {
                // Login call has to happen on UI thread, so circle back around to it
                CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
                    Windows::UI::Core::CoreDispatcherPriority::Normal,
                    ref new Windows::UI::Core::DispatchedHandler([=]()
                {
                    TryRerequest(false);
                }));
            }
            else
            {
                NavigateToOptionsPage();
            }
        }
    });
}

void MainPage::LogoutAndRetry(
    )
{
    // It's necessary to logout prior to reattempting login, because it could
    // be that the session has cached an access token that is no longer valid,
    // e.g. if the user revoked the app in Settings.  FBSession::Logout clears
    // the session's cached access token, among other things.
    create_task(FBSession::ActiveSession->Logout())
        .then([=]()
    {
        // Login call has to happen on UI thread, so circle back around to it
        CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
            Windows::UI::Core::CoreDispatcherPriority::Normal,
            ref new Windows::UI::Core::DispatchedHandler([=]()
        {
            TryRerequest(TRUE);
        }));
    });
}

void MainPage::login_OnClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    FBSession^ sess = FBSession::ActiveSession;

    if (sess->LoggedIn)
    {
        sess->Logout();
        LoginButton->Content = L"Logout";
        LoginCpp::App^ a = dynamic_cast<LoginCpp::App^>(Application::Current);
        Windows::UI::Xaml::Controls::Frame^ f = a->CreateRootFrame();
        f->Navigate(MainPage::typeid);
    }
    else
    {
        create_task(sess->LoginAsync(BuildPermissions())).then([=](FBResult^ result)
        {
            // There may be other cases where an a failed login request should
            // prompt the app to retry login, but this one is common enough that
            // it's helpful to demonstrate handling it here.  If the predicate
            // returns TRUE, the user has gone to facebook.com in the browser,
            // and removed our app from their list off allowed apps in Settings.
            if (WasAppPermissionRemovedByUser(result))
            {
                LogoutAndRetry();
            }
            else if (ShouldRerequest(result))
            {
                // Login call has to happen on UI thread, so circle back around to it
                CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
                    Windows::UI::Core::CoreDispatcherPriority::Normal,
                    ref new Windows::UI::Core::DispatchedHandler([=]()
                {
                    TryRerequest(FALSE);
                }));
            }
            else if (result && result->Succeeded)
            {
                // Got a token, and all the permissions we wanted - go ahead to the options page
                NavigateToOptionsPage();
            }
        });
    }
}
