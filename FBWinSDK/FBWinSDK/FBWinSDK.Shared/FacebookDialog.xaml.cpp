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
#define FACEBOOK_DIALOG_CLOSE_PATH   L"/dialog/close"

const wchar_t* ErrorObjectJson = L"{\"error\": {\"message\": " \
L"\"Operation Canceled\", \"type\": " \
L"\"OAuthException\", \"code\": 4201, " \
L"\"error_user_msg\": \"User canceled the Dialog flow\"" \
L"}}";


#ifdef _DEBUG
void DebugPrintLine(
    String^ msg
    )
{
    String^ output = msg + L"\n";
    OutputDebugString(output->Data());
}
#else
#define DebugPrintLine(msg) ((void) 0)
#endif

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236
FacebookDialog::FacebookDialog()
{
    InitializeComponent();
    InitDialog();
}

FacebookDialog::~FacebookDialog(
    )
{
    DebugPrintLine(L"FacebookDialog dtor");
}

void FacebookDialog::InitDialog()
{
    _popup = nullptr;
    _dialogResponse = nullptr;

    CoreWindow^ wnd1 = CoreApplication::MainView->CoreWindow;

    _popup = ref new Popup();

    _popup->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Stretch;
    _popup->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Stretch;

    Margin = Windows::UI::Xaml::Thickness(0, 0, 0, 0);
    Height = wnd1->Bounds.Height;
    Width = wnd1->Bounds.Width;

    sizeChangedEventRegistrationToken =  wnd1->SizeChanged += 
        ref new TypedEventHandler<CoreWindow ^, WindowSizeChangedEventArgs ^>
            (this, &FacebookDialog::OnSizeChanged);

    _popup->Child = this;
}

void FacebookDialog::UninitDialog()
{
    dialogWebBrowser->NavigationStarting -= navigatingEventHandlerRegistrationToken;
    CoreApplication::MainView->CoreWindow->SizeChanged -= 
        sizeChangedEventRegistrationToken;

    _popup->IsOpen = false;

    //
    // This breaks the circular dependency between the popup and dialog 
    // class, and is essential in order for the dialog to be disposed of
    // properly.
    //
    _popup->Child = nullptr;
}

FBResult^ FacebookDialog::GetDialogResponse()
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

void FacebookDialog::ShowDialog(
    DialogUriBuilder^ uriBuilder,
    TypedEventHandler<WebView^, WebViewNavigationStartingEventArgs^>^ EventHandler,
    PropertySet^ Parameters
    )
{
    _dialogResponse = nullptr;
    Uri^ dialogUrl = uriBuilder(Parameters);

    navigatingEventHandlerRegistrationToken = dialogWebBrowser->NavigationStarting +=
        EventHandler;

    _popup->IsOpen = true;

    dialogWebBrowser->Navigate(dialogUrl);
}

void FacebookDialog::ShowLoginDialog(
    PropertySet^ Parameters
    )
{
    TypedEventHandler<WebView^, WebViewNavigationStartingEventArgs^>^ handler =
        ref new TypedEventHandler<WebView^, WebViewNavigationStartingEventArgs^>(
            this, &FacebookDialog::dialogWebView_LoginNavStarting);
    ShowDialog(ref new DialogUriBuilder(this,
        &FacebookDialog::BuildLoginDialogUrl), handler, Parameters);
}

void FacebookDialog::ShowFeedDialog(
    PropertySet^ Parameters
    )
{
    TypedEventHandler<WebView^, WebViewNavigationStartingEventArgs^>^ handler =
        ref new TypedEventHandler<WebView^, WebViewNavigationStartingEventArgs^>(
            this, &FacebookDialog::dialogWebView_FeedNavStarting);
    ShowDialog(ref new DialogUriBuilder(this,
        &FacebookDialog::BuildFeedDialogUrl), handler, Parameters);
}

void FacebookDialog::ShowRequestsDialog(
    PropertySet^ Parameters
    )
{
    TypedEventHandler<WebView^, WebViewNavigationStartingEventArgs^>^ handler =
        ref new TypedEventHandler<WebView^, WebViewNavigationStartingEventArgs^>(
            this, &FacebookDialog::dialogWebView_RequestNavStarting);
    ShowDialog(ref new DialogUriBuilder(this,
        &FacebookDialog::BuildRequestsDialogUrl), handler, Parameters);
}

String^ FacebookDialog::GetRedirectUriString(
    String^ FacebookDialogName
    )
{
    FBSession^ sess = FBSession::ActiveSession;

    //
    // This looks strange, but is correct.  One side or the other of this 
    // conversation has a problem with all the other types of redirect
    // protocol/URIs accepted for apps, so we're left with always redirecting
    // to the login_success page on FB, then canceling the redirect in our
    // NavigationStarted event handler, for all dialogs.
    // 
    String^ result = L"https://" + GetFBServer() + L".facebook.com" +
        FACEBOOK_LOGIN_SUCCESS_PATH;
        
    result = Uri::EscapeComponent(result);

    DebugPrintLine(L"Redirect URI is " + result);
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

#define ScopeKey        L"scope"
#define DisplayKey      L"display"
#define ResponseTypeKey L"response_type"
#define DefaultScope    L"public_profile,email,user_friends"
#define DefaultDisplay  L"popup"
#define DefaultResponse L"token"

Uri^ FacebookDialog::BuildLoginDialogUrl(
    PropertySet^ Parameters
    )
{
    FBSession^ s = FBSession::ActiveSession;
    String^ uriString = L"https://" + GetFBServer() +
        L".facebook.com/dialog/oauth?client_id=" + s->FBAppId;

    // Use some reasonable default login parameters
    String^ scope = DefaultScope;
    String^ displayType = DefaultDisplay;
    String^ responseType = DefaultResponse;

    uriString += L"&redirect_uri=" + GetRedirectUriString(L"login");

    // Enumerate through all the parameters
    IIterator<IKeyValuePair<String^, Object^>^>^ first = Parameters->First();
    while (first && (first->HasCurrent))
    {
        String^ Key = first->Current->Key;
        String^ Value = dynamic_cast<String^>(first->Current->Value);
        if (Value)
        {
            if (!String::CompareOrdinal(Key, ScopeKey))
            {
                scope = Value;
            }
            else if (!String::CompareOrdinal(Key, DisplayKey))
            {
                displayType = Value;
            }
            else if (!String::CompareOrdinal(Key, ResponseTypeKey))
            {
                responseType = Value;
            }
            else
            {
                uriString += L"&" + Key + L"=" + Value;
            }
        }

        first->MoveNext();
    }

    uriString += L"&" + ScopeKey + L"=" + scope + L"&" + DisplayKey + L"=" +
        displayType + L"&" + ResponseTypeKey + L"=" + responseType;

    return ref new Uri(uriString);
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

bool FacebookDialog::IsDialogCloseRedirect(
    Uri^ Response
    )
{
    return (String::CompareOrdinal(Response->Path, FACEBOOK_DIALOG_CLOSE_PATH) == 0);
}

void FacebookDialog::dialogWebView_LoginNavStarting(
    WebView^ sender, 
    WebViewNavigationStartingEventArgs^ e
    )
{
    DebugPrintLine(L"Navigating to " + e->Uri->DisplayUri);
    DebugPrintLine(L"Path is " + e->Uri->Path);

    if (IsLoginSuccessRedirect(e->Uri))
    {
        dialogWebBrowser->Stop();

        UninitDialog();

        FBAccessTokenData^ tokenData = FBAccessTokenData::FromUri(e->Uri);
        if (tokenData)
        {
            _dialogResponse = ref new FBResult(tokenData);
        }
        else
        {
            FBError^ err = FBError::FromJson(ref new String(ErrorObjectJson));
            _dialogResponse = ref new FBResult(err);
        }
    }
    else if (IsDialogCloseRedirect(e->Uri))
    {
        dialogWebBrowser->Stop();

        UninitDialog();

        FBError^ err = FBError::FromJson(ref new String(ErrorObjectJson));
        _dialogResponse = ref new FBResult(err);
    }
}

void FacebookDialog::dialogWebView_FeedNavStarting(
    WebView^ sender,
    WebViewNavigationStartingEventArgs^ e
    )
{
    DebugPrintLine(L"Navigating to " + e->Uri->DisplayUri);
    DebugPrintLine(L"Path is " + e->Uri->Path);

    if (IsLoginSuccessRedirect(e->Uri))
    {
        dialogWebBrowser->Stop();

        UninitDialog();

        DebugPrintLine(L"Feed response is " + e->Uri->DisplayUri);

        FBFeedRequest^ request = FBFeedRequest::FromFeedDialogResponse(e->Uri);
        if (request)
        {
            _dialogResponse = ref new FBResult(request);
        }
        else
        {
            FBError^ err = FBError::FromJson(ref new String(ErrorObjectJson));
            _dialogResponse = ref new FBResult(err);
        }
    }
    else if (IsDialogCloseRedirect(e->Uri))
    {
        dialogWebBrowser->Stop();

        UninitDialog();

        FBError^ err = FBError::FromJson(ref new String(ErrorObjectJson));
        _dialogResponse = ref new FBResult(err);
    }
}

void FacebookDialog::dialogWebView_RequestNavStarting(
    WebView^ sender,
    WebViewNavigationStartingEventArgs^ e
    )
{
    DebugPrintLine(L"Navigating to " + e->Uri->DisplayUri);
    DebugPrintLine(L"Path is " + e->Uri->Path);

    if (IsLoginSuccessRedirect(e->Uri))
    {
        dialogWebBrowser->Stop();

        UninitDialog();

        DebugPrintLine(L"Request response is " + e->Uri->DisplayUri);

        FBAppRequest^ request = FBAppRequest::FromRequestDialogResponse(e->Uri);
        if (request)
        {
            _dialogResponse = ref new FBResult(request);
        }
        else
        {
            FBError^ err = FBError::FromJson(ref new String(ErrorObjectJson));
            _dialogResponse = ref new FBResult(err);
        }
    }
    else if (IsDialogCloseRedirect(e->Uri))
    {
        dialogWebBrowser->Stop();

        UninitDialog();

        FBError^ err = FBError::FromJson(ref new String(ErrorObjectJson));
        _dialogResponse = ref new FBResult(err);
    }
}

void FacebookDialog::CloseDialogButton_OnClick(
    Object^ sender, 
    RoutedEventArgs^ e
    )
{
    _popup->IsOpen = false;

    FBError^ err = FBError::FromJson(ref new String(ErrorObjectJson));
    _dialogResponse = ref new FBResult(err);
    _popup->Child = nullptr;
}

void FacebookDialog::OnSizeChanged(
    CoreWindow ^sender, 
    WindowSizeChangedEventArgs ^args
    )
{
    Height = sender->Bounds.Height;
    Width = sender->Bounds.Width;
}

