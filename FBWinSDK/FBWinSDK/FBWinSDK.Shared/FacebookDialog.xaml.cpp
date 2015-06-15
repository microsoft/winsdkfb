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
// FacebookDialog.xaml.cpp
// Implementation of the FacebookDialog class
//

#include "pch.h"
#include "FacebookDialog.xaml.h"
#include "FacebookSession.h"
#include "FacebookFeedRequest.h"
#include "FacebookAppRequest.h"
#include "FacebookClient.h"

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Popups;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Facebook;
using namespace Facebook::Graph;

#if (defined(_MSC_VER) && (_MSC_VER >= 1800)) 
using namespace concurrency;
#else
using namespace pplx;
#endif

using namespace std;

#define FACEBOOK_DESKTOP_SERVER_NAME L"www"
#define FACEBOOK_MOBILE_SERVER_NAME  L"m"
#define FACEBOOK_LOGIN_SUCCESS_PATH  L"/connect/login_success.html"

void DebugSpew(
    String^ msg
    )
{
    String^ output = msg + L"\n";
    OutputDebugString(output->Data());
}

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236
FacebookDialog::FacebookDialog()
{
    InitializeComponent();
    _popup = nullptr;
    _dialogResponse = nullptr;

    CoreWindow^ wnd1 = CoreApplication::MainView->CoreWindow;

    _popup = ref new Popup();

    _popup->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Stretch;
    _popup->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Stretch;

    Margin = Windows::UI::Xaml::Thickness(0, 0, 0, 0);
    Height = wnd1->Bounds.Height;
    Width = wnd1->Bounds.Width;

    wnd1->SizeChanged += ref new TypedEventHandler<CoreWindow ^, WindowSizeChangedEventArgs ^>
            (this, &FacebookDialog::OnSizeChanged);

    _popup->Child = this;
}

FBResult^ FacebookDialog::DialogResponse::get()
{
    FBResult^ response = _dialogResponse;
    if (response)
    {
        // Caller (FBSession) only gets the response *once*.  Then we reset
        // it to null, so it's valid to poll on the value being null again.
        _dialogResponse = nullptr;
    }

    return response;
}

void FacebookDialog::ShowLoginDialog(
    )
{
    _dialogResponse = nullptr;
    Uri^ loginDialogUrl = BuildLoginDialogUrl();
    navigatingEventHandlerRegistrationToken = dialogWebBrowser->NavigationStarting +=
        ref new TypedEventHandler<WebView^, WebViewNavigationStartingEventArgs^>(
            this, &FacebookDialog::dialogWebView_LoginNavStarting);

    _popup->IsOpen = true;

    dialogWebBrowser->Navigate(loginDialogUrl);
}

void FacebookDialog::ShowFeedDialog(
    PropertySet^ Parameters
    )
{
    _dialogResponse = nullptr;
    Uri^ feedDialogUrl = BuildFeedDialogUrl(Parameters);
    navigatingEventHandlerRegistrationToken = dialogWebBrowser->NavigationStarting += 
        ref new TypedEventHandler<WebView^, WebViewNavigationStartingEventArgs^>(
            this, &FacebookDialog::dialogWebView_FeedNavStarting);

    _popup->IsOpen = true;

    dialogWebBrowser->Navigate(feedDialogUrl);
}

void FacebookDialog::ShowRequestsDialog(
    PropertySet^ Parameters
    )
{
    _dialogResponse = nullptr;
    Uri^ requestDialogUrl = BuildRequestsDialogUrl(Parameters);
    navigatingEventHandlerRegistrationToken = dialogWebBrowser->NavigationStarting +=
        ref new TypedEventHandler<WebView^, WebViewNavigationStartingEventArgs^>(
            this, &FacebookDialog::dialogWebView_RequestNavStarting);

    _popup->IsOpen = true;

    dialogWebBrowser->Navigate(requestDialogUrl);
}

String^ FacebookDialog::GetRedirectUriString(
    String^ FacebookDialogName
    )
{
    FBSession^ sess = FBSession::ActiveSession;

    String^ result = L"https://" + GetFBServer() + L".facebook.com" +
        FACEBOOK_LOGIN_SUCCESS_PATH;
        
    result = Uri::EscapeComponent(result);

    DebugSpew(L"Redirect URI is " + result);
    return result;
}

BOOL FacebookDialog::IsMobilePlatform(
    )
{
    BOOL isMobile = FALSE;
#if defined(_WIN32_WINNT_WIN10) && (_WIN32_WINNT >= _WIN32_WINNT_WIN10)
    //TODO: Detect mobile/desktop on Win10.  Defaulting to desktop for now.
#endif
#if WINAPI_FAMILY==WINAPI_FAMILY_PHONE_APP
    isMobile = TRUE;
#endif
    return isMobile;
}

String^ FacebookDialog::GetFBServer(
    )
{
    String^ server = nullptr;

    if (IsMobilePlatform())
    {
        server = FACEBOOK_MOBILE_SERVER_NAME;
    }
    else
    {
        server = FACEBOOK_DESKTOP_SERVER_NAME;
    }

    return server;
}

Uri^ FacebookDialog::BuildLoginDialogUrl(
    )
{
    FBSession^ sess = FBSession::ActiveSession;
    String^ dialogUriString =
        L"https://" + GetFBServer() + 
        L".facebook.com/dialog/oauth?client_id=" + sess->FBAppId + 
        L"&redirect_uri=" + GetRedirectUriString(L"login") + L"&app_id=" + 
        sess->FBAppId + L"&scope=" + sess->PermissionsToString() + 
        L"&display=popup" + L"&response_type=token";

    DebugSpew(L"Request string is " + dialogUriString);

    return ref new Uri(dialogUriString);
}

Uri^ FacebookDialog::BuildFeedDialogUrl(
    PropertySet^ Parameters
    )
{
    FBSession^ sess = FBSession::ActiveSession;
    String^ dialogUriString =
        L"https://" + GetFBServer() + 
        L".facebook.com/v2.1/dialog/feed?access_token=" +
        sess->AccessTokenData->AccessToken +
        L"&redirect_uri=" + GetRedirectUriString(L"feed") +
        L"&app_id=" + sess->FBAppId; 
    String^ queryString = FBClient::ParametersToQueryString(Parameters);
    if (queryString->Length() > 0)
    {
        dialogUriString += "&" + queryString;
    }

    return ref new Uri(dialogUriString);
}

Uri^ FacebookDialog::BuildRequestsDialogUrl(
    PropertySet^ Parameters
    )
{
    FBSession^ sess = FBSession::ActiveSession;
    String^ dialogUriString =
        L"https://" + GetFBServer() + 
        L".facebook.com/v2.1/dialog/apprequests?access_token=" +
        sess->AccessTokenData->AccessToken +
        L"&redirect_uri=" + GetRedirectUriString(L"requests") +
        L"&app_id=" + sess->FBAppId;
    String^ queryString = FBClient::ParametersToQueryString(Parameters);
    if (queryString->Length() > 0)
    {
        dialogUriString += "&" + queryString;
    }

    return ref new Uri(dialogUriString);
}

bool FacebookDialog::IsLoginSuccessRedirect(
    Uri^ Response
    )
{
    return (String::CompareOrdinal(Response->Path, FACEBOOK_LOGIN_SUCCESS_PATH) == 0);
}

void FacebookDialog::dialogWebView_LoginNavStarting(
    WebView^ sender, 
    WebViewNavigationStartingEventArgs^ e
    )
{
    DebugSpew(L"Navigating to " + e->Uri->DisplayUri);
    DebugSpew(L"Path is " + e->Uri->Path);

    if (IsLoginSuccessRedirect(e->Uri))
    {
        dialogWebBrowser->Stop();
            
        _popup->IsOpen = false;

        FBAccessTokenData^ tokenData = FBAccessTokenData::FromUri(e->Uri);
        if (tokenData)
        {
            _dialogResponse = ref new FBResult(tokenData);
        }
        else
        {
            String^ ErrorObjectJson = L"{\"error\": {\"message\": "
                L"\"Operation Canceled\", \"type\": "
                L"\"OAuthException\", \"code\": 4201, "
                L"\"error_user_msg\": \"User canceled the Dialog flow\""
                L"}}";

            FBError^ err = FBError::FromJson(ErrorObjectJson);
            _dialogResponse = ref new FBResult(err);
        }

        // deregister the event handler
        dialogWebBrowser->NavigationStarting -= navigatingEventHandlerRegistrationToken;
    }
}

void FacebookDialog::dialogWebView_FeedNavStarting(
    WebView^ sender,
    WebViewNavigationStartingEventArgs^ e
    )
{
    DebugSpew(L"Navigating to " + e->Uri->DisplayUri);
    DebugSpew(L"Path is " + e->Uri->Path);

    if (IsLoginSuccessRedirect(e->Uri))
    {
        dialogWebBrowser->Stop();

        _popup->IsOpen = false;

        DebugSpew(L"Feed response is " + e->Uri->DisplayUri);

        FBFeedRequest^ request = FBFeedRequest::FromFeedDialogResponse(e->Uri);
        if (request)
        {
            _dialogResponse = ref new FBResult(request);
        }
        else
        {
            String^ ErrorObjectJson = L"{\"error\": {\"message\": "
                L"\"Operation Canceled\", \"type\": "
                L"\"OAuthException\", \"code\": 4201, "
                L"\"error_user_msg\": \"User canceled the Dialog flow\""
                L"}}";

            FBError^ err = FBError::FromJson(ErrorObjectJson);
            _dialogResponse = ref new FBResult(err);
        }

        // deregister the event handler
        dialogWebBrowser->NavigationStarting -= navigatingEventHandlerRegistrationToken;
    }
}

void FacebookDialog::dialogWebView_RequestNavStarting(
    WebView^ sender,
    WebViewNavigationStartingEventArgs^ e
    )
{
    DebugSpew(L"Navigating to " + e->Uri->DisplayUri);
    DebugSpew(L"Path is " + e->Uri->Path);

    if (IsLoginSuccessRedirect(e->Uri))
    {
        dialogWebBrowser->Stop();

        _popup->IsOpen = false;

        DebugSpew(L"Request response is " + e->Uri->DisplayUri);

        FBAppRequest^ request = FBAppRequest::FromRequestDialogResponse(e->Uri);
        if (request)
        {
            _dialogResponse = ref new FBResult(request);
        }
        else
        {
            String^ ErrorObjectJson = L"{\"error\": {\"message\": "
                L"\"Operation Canceled\", \"type\": "
                L"\"OAuthException\", \"code\": 4201, "
                L"\"error_user_msg\": \"User canceled the Dialog flow\""
                L"}}";

            FBError^ err = FBError::FromJson(ErrorObjectJson);
            _dialogResponse = ref new FBResult(err);
        }

        // deregister the event handler
        dialogWebBrowser->NavigationStarting -= navigatingEventHandlerRegistrationToken;
    }
}

void FacebookDialog::CloseDialogButton_OnClick(
    Object^ sender, 
    RoutedEventArgs^ e
    )
{
    _popup->IsOpen = false;
}

void FacebookDialog::OnSizeChanged(
    CoreWindow ^sender, 
    WindowSizeChangedEventArgs ^args
    )
{
    Height = sender->Bounds.Height;
    Width = sender->Bounds.Width;
}

