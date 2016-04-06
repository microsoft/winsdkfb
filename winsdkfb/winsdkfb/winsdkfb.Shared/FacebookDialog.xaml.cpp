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
#include "FacebookSendRequest.h"
#include "FacebookClient.h"
#include "DialogUrlBuilder.h"

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Web::Http;
using namespace Windows::Web::Http::Filters;
using namespace Windows::UI::Popups;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace winsdkfb;
using namespace winsdkfb::Graph;

#if (defined(_MSC_VER) && (_MSC_VER >= 1800)) 
using namespace concurrency;
#else
using namespace pplx;
#endif

using namespace std;

const wchar_t* ErrorObjectJson = L"{\"error\": {\"message\": " \
L"\"Operation Canceled\", \"type\": " \
L"\"OAuthException\", \"code\": 4201, " \
L"\"error_user_msg\": \"User canceled the Dialog flow\"" \
L"}}";

const wchar_t* ErrorObjectJsonLogout = L"{\"error\": {\"message\": " \
L"\"Operation Canceled\", \"type\": " \
L"\"OAuthException\", \"code\": 4202, " \
L"\"error_user_msg\": \"User logged out\"" \
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
    dialogWebBrowser->Stop();
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

IAsyncOperation<FBResult^>^ FacebookDialog::ShowDialog(
    DialogUriBuilder^ uriBuilder,
    TypedEventHandler<WebView^, WebViewNavigationStartingEventArgs^>^ EventHandler,
    PropertySet^ Parameters
    )
{
    Uri^ dialogUrl = uriBuilder(Parameters);

    navigatingEventHandlerRegistrationToken = dialogWebBrowser->NavigationStarting +=
        EventHandler;

    _popup->IsOpen = true;

    dialogWebBrowser->Navigate(dialogUrl);
    dialogWebBrowser->Focus(Windows::UI::Xaml::FocusState::Programmatic);

    return create_async([=]()
    {
        return create_task(_dialogResponse);
    });
}

IAsyncOperation<FBResult^>^ FacebookDialog::ShowLoginDialog(
    PropertySet^ Parameters
    )
{
    TypedEventHandler<WebView^, WebViewNavigationStartingEventArgs^>^ handler =
        ref new TypedEventHandler<WebView^, WebViewNavigationStartingEventArgs^>(
            this, &FacebookDialog::dialogWebView_LoginNavStarting);
    return ShowDialog(ref new DialogUriBuilder(this,
        &FacebookDialog::BuildLoginDialogUrl), handler, Parameters);
}

IAsyncOperation<FBResult^>^ FacebookDialog::ShowFeedDialog(
    PropertySet^ Parameters
    )
{
    TypedEventHandler<WebView^, WebViewNavigationStartingEventArgs^>^ handler =
        ref new TypedEventHandler<WebView^, WebViewNavigationStartingEventArgs^>(
            this, &FacebookDialog::dialogWebView_FeedNavStarting);
    return ShowDialog(ref new DialogUriBuilder(
        &DialogUrlBuilder::BuildFeedDialogUrl), handler, Parameters);
}

IAsyncOperation<FBResult^>^ FacebookDialog::ShowRequestsDialog(
    PropertySet^ Parameters
    )
{
    TypedEventHandler<WebView^, WebViewNavigationStartingEventArgs^>^ handler =
        ref new TypedEventHandler<WebView^, WebViewNavigationStartingEventArgs^>(
            this, &FacebookDialog::dialogWebView_RequestNavStarting);
    return ShowDialog(ref new DialogUriBuilder(
        &DialogUrlBuilder::BuildRequestsDialogUrl), handler, Parameters);
}

IAsyncOperation<FBResult^>^ FacebookDialog::ShowSendDialog(
    PropertySet^ Parameters
    )
{
    TypedEventHandler<WebView^, WebViewNavigationStartingEventArgs^>^ handler =
        ref new TypedEventHandler<WebView^, WebViewNavigationStartingEventArgs^>(
            this, &FacebookDialog::dialogWebView_SendNavStarting);
    return ShowDialog(ref new DialogUriBuilder(
        &DialogUrlBuilder::BuildSendDialogUrl), handler, Parameters);
}

void FacebookDialog::DeleteCookies()
{
	// This allows on WP8.1 to logIn with other account from the webView
	// and on W8.1 & W10 to logIn with other account when the 'Keep me logged in' option from webView was selected
	HttpBaseProtocolFilter^ filter = ref new HttpBaseProtocolFilter();
	HttpCookieManager^ cookieManager = filter->CookieManager;
	HttpCookieCollection^ cookiesJar = cookieManager->GetCookies(ref new Uri(FacebookDialog::GetFBServerUrl()));
	for (HttpCookie^ cookie : cookiesJar)
	{
		cookieManager->DeleteCookie(cookie);
	}
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

String^ FacebookDialog::GetFBServerUrl(
    )
{
    String^ server = nullptr;

    if (FacebookDialog::IsMobilePlatform())
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
    PropertySet^ Parameters
    )
{
    FBSession^ s = FBSession::ActiveSession;
    String^ apiVersion = L"";
    if (s->APIMajorVersion)
    {
        apiVersion = L"/v" + s->APIMajorVersion.ToString() + L"." + s->APIMinorVersion.ToString() + L"/";
    }
    String^ uriString = FacebookDialog::GetFBServerUrl() + apiVersion + L"dialog/oauth?client_id=" + s->FBAppId;

    // Use some reasonable default login parameters
    String^ scope = DefaultScope;
    String^ displayType = DefaultDisplay;
    String^ responseType = DefaultResponse;

    uriString += L"&redirect_uri=" + DialogUrlBuilder::GetRedirectUriString(L"login");

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


bool FacebookDialog::IsLoginSuccessRedirect(
    Uri^ Response
    )
{
    return (String::CompareOrdinal(Response->Path, FACEBOOK_LOGIN_SUCCESS_PATH) == 0);
}

bool FacebookDialog::IsLogoutRedirect(
    Uri^ Response
    )
{
    return (String::CompareOrdinal(Response->Path, FACEBOOK_LOGOUT_PATH) == 0);
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
        UninitDialog();

        FBAccessTokenData^ tokenData = FBAccessTokenData::FromUri(e->Uri);
        if (tokenData)
        {
            SetDialogResponse(ref new FBResult(tokenData));
        }
        else
        {
            FBError^ err = FBError::FromJson(ref new String(ErrorObjectJson));
            SetDialogResponse(ref new FBResult(err));
        }
    }
    else if (IsDialogCloseRedirect(e->Uri))
    {
        UninitDialog();

        FBError^ err = FBError::FromJson(ref new String(ErrorObjectJson));
        SetDialogResponse(ref new FBResult(err));
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
        UninitDialog();

        DebugPrintLine(L"Feed response is " + e->Uri->DisplayUri);

        FBFeedRequest^ request = FBFeedRequest::FromFeedDialogResponse(e->Uri);
        if (request)
        {
            SetDialogResponse(ref new FBResult(request));
        }
        else
        {
            FBError^ err = FBError::FromJson(ref new String(ErrorObjectJson));
            SetDialogResponse(ref new FBResult(err));
        }
    }
    else if (IsLogoutRedirect(e->Uri))
    {
        UninitDialog();

        DebugPrintLine(L"Feed response is " + e->Uri->DisplayUri);
        FBSession^ sess = FBSession::ActiveSession;
        sess->LogoutAsync();

        FBError^ err = FBError::FromJson(ref new String(ErrorObjectJsonLogout));
        SetDialogResponse(ref new FBResult(err));
    }
    else if (IsDialogCloseRedirect(e->Uri))
    {
        UninitDialog();

        FBError^ err = FBError::FromJson(ref new String(ErrorObjectJson));
        SetDialogResponse(ref new FBResult(err));
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
        UninitDialog();

        DebugPrintLine(L"Request response is " + e->Uri->DisplayUri);

        FBAppRequest^ request = FBAppRequest::FromRequestDialogResponse(e->Uri);
        if (request)
        {
            SetDialogResponse(ref new FBResult(request));
        }
        else
        {
            FBError^ err = FBError::FromJson(ref new String(ErrorObjectJson));
            SetDialogResponse(ref new FBResult(err));
        }
    }
    else if (IsLogoutRedirect(e->Uri))
    {
        UninitDialog();

        DebugPrintLine(L"Request response is " + e->Uri->DisplayUri);
        FBSession^ sess = FBSession::ActiveSession;
        sess->LogoutAsync();

        FBError^ err = FBError::FromJson(ref new String(ErrorObjectJsonLogout));
        SetDialogResponse(ref new FBResult(err));
    }
    else if (IsDialogCloseRedirect(e->Uri))
    {
        UninitDialog();

        FBError^ err = FBError::FromJson(ref new String(ErrorObjectJson));
        SetDialogResponse(ref new FBResult(err));
    }
}

void FacebookDialog::dialogWebView_SendNavStarting(
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
        SetDialogResponse(ref new FBResult(ref new FBSendRequest()));

    }
    else if (IsLogoutRedirect(e->Uri))
    {
        UninitDialog();

        DebugPrintLine(L"Request response is " + e->Uri->DisplayUri);
        FBSession^ sess = FBSession::ActiveSession;
        sess->LogoutAsync();

        FBError^ err = FBError::FromJson(ref new String(ErrorObjectJsonLogout));
        SetDialogResponse(ref new FBResult(err));
    }
    else if (IsDialogCloseRedirect(e->Uri))
    {
        dialogWebBrowser->Stop();

        UninitDialog();

        FBError^ err = FBError::FromJson(ref new String(ErrorObjectJson));
        SetDialogResponse(ref new FBResult(err));
    }
}

void FacebookDialog::CloseDialogButton_OnClick(
    Object^ sender, 
    RoutedEventArgs^ e
    )
{
    UninitDialog();

    FBError^ err = FBError::FromJson(ref new String(ErrorObjectJson));
    SetDialogResponse(ref new FBResult(err));
}

void FacebookDialog::OnSizeChanged(
    CoreWindow ^sender, 
    WindowSizeChangedEventArgs ^args
    )
{
    Height = sender->Bounds.Height;
    Width = sender->Bounds.Width;
}

void FacebookDialog::SetDialogResponse(
    FBResult^ dialogResponse
    )
{
    _dialogResponse.set(dialogResponse);
}

