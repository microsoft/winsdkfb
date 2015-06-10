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

using namespace Platform;
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

void DebugSpew(
    String^ msg
    )
{
    String^ output = msg + L"\n";
    OutputDebugString(output->Data());
}

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236
namespace Facebook
{
    FacebookDialog::FacebookDialog()
    {
        InitializeComponent();
        _popup = nullptr;
        _dialogResponse = nullptr;

        CoreWindow^ wnd1 = CoreApplication::MainView->CoreWindow;

        _grid = ref new Grid();
        _popup = ref new Popup();

        _popup->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Stretch;
        _popup->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Stretch;

        // TODO: (sanjeevd) hard coded, remove and fix these
        _grid->Margin = Windows::UI::Xaml::Thickness(0, 0, 0, 0);
        _grid->Height = wnd1->Bounds.Height;
        _grid->Width = wnd1->Bounds.Width;

        Width = _grid->Width;

        _grid->Children->Append(this);
        _popup->Child = _grid;
    }

    extern HANDLE login_evt;

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

        CoreWindow^ wnd1 = CoreApplication::MainView->CoreWindow;
        _grid->Height = wnd1->Bounds.Height;
        _grid->Width = wnd1->Bounds.Width;
        Width = _grid->Width;
        _popup->IsOpen = true;

        dialogWebBrowser->Navigate(loginDialogUrl);
    }

    void FacebookDialog::ShowFeedDialog(
        )
    {
        _dialogResponse = nullptr;
        Uri^ feedDialogUrl = BuildFeedDialogUrl();
        navigatingEventHandlerRegistrationToken = dialogWebBrowser->NavigationStarting += 
            ref new TypedEventHandler<WebView^, WebViewNavigationStartingEventArgs^>(
                this, &FacebookDialog::dialogWebView_FeedNavStarting);

        CoreWindow^ wnd1 = CoreApplication::MainView->CoreWindow;
        _grid->Height = wnd1->Bounds.Height;
        _grid->Width = wnd1->Bounds.Width;
        Width = _grid->Width;
        _popup->IsOpen = true;

        dialogWebBrowser->Navigate(feedDialogUrl);
    }

    void FacebookDialog::ShowRequestsDialog(
        )
    {
        _dialogResponse = nullptr;
        Uri^ requestDialogUrl = BuildRequestsDialogUrl();
        navigatingEventHandlerRegistrationToken = dialogWebBrowser->NavigationStarting +=
            ref new TypedEventHandler<WebView^, WebViewNavigationStartingEventArgs^>(
                this, &FacebookDialog::dialogWebView_RequestNavStarting);

        CoreWindow^ wnd1 = CoreApplication::MainView->CoreWindow;
        _grid->Height = wnd1->Bounds.Height;
        _grid->Width = wnd1->Bounds.Width;
        Width = _grid->Width;
        _popup->IsOpen = true;

        dialogWebBrowser->Navigate(requestDialogUrl);
    }

    String^ FacebookDialog::GetRedirectUriString(
        String^ FacebookDialogName
        )
    {
        FBSession^ sess = FBSession::ActiveSession;

        //String^ result = L"fb" + sess->FBAppId + "%3A%2F%2F" +
        //    FacebookDialogName + "_success&app_id=" +
        //    sess->FBAppId + L"&display=touch";

        String^ result = L"https%3A%2F%2Fwww.facebook.com%2Fconnect%2F" +
            "login_success.html&app_id=" +
            sess->FBAppId;

        DebugSpew(L"Redirect URI is " + result);
        return result;
    }

    Uri^ FacebookDialog::BuildLoginDialogUrl(
        )
    {
        FBSession^ sess = FBSession::ActiveSession;
        String^ dialogUriString =
            L"https://www.facebook.com/dialog/oauth?client_id=" +
            sess->FBAppId + L"&redirect_uri=" + 
            GetRedirectUriString(L"login") + L"&scope="
            + sess->PermissionsToString() + L"&display=popup" +
            L"&response_type=token";

        //String^ dialogUriString =
        //    L"https://m.facebook.com/dialog/oauth?client_id=" +
        //    sess->FBAppId + L"&redirect_uri=" +
        //    GetRedirectUriString(L"login") + L"&scope="
        //    + sess->PermissionsToString() + L"&display=popup" +
        //    L"&response_type=token";

        DebugSpew(L"Request string is " + dialogUriString);

        return ref new Uri(dialogUriString);
    }

    Uri^ FacebookDialog::BuildFeedDialogUrl(
        )
    {
        FBSession^ sess = FBSession::ActiveSession;
        String^ dialogUriString =
            L"https://www.facebook.com/v2.1/dialog/feed?access_token=" +
            sess->AccessTokenData->AccessToken +
            L"&redirect_uri=" + GetRedirectUriString(L"feed"); 

        //String^ dialogUriString =
        //    L"https://m.facebook.com/v2.1/dialog/feed?access_token=" +
        //    sess->AccessTokenData->AccessToken +
        //    L"&redirect_uri=" + GetRedirectUriString(L"feed");

        return ref new Uri(dialogUriString);
    }

    Uri^ FacebookDialog::BuildRequestsDialogUrl(
        )
    {
        FBSession^ sess = FBSession::ActiveSession;
        //TODO: YOUR_MESSAGE_HERE?  Do we need a message parameter here?  It 
        //doesn't appear to show up anywhere in the requests dialog...
        String^ dialogUriString =
            L"https://www.facebook.com/v2.1/dialog/apprequests?access_token=" +
            sess->AccessTokenData->AccessToken +
            L"&redirect_uri=" + GetRedirectUriString(L"requests") +
            L"&message=YOUR_MESSAGE_HERE";

        return ref new Uri(dialogUriString);
    }

    bool FacebookDialog::IsLoginSuccessRedirect(
        Uri^ Response
        )
    {
        return (String::CompareOrdinal(Response->Path, L"/connect/login_success.html") == 0);
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

            String^ ErrorObjectJson = L"{\"error\": {\"message\": "
                L"\"Operation Canceled\", \"type\": "
                L"\"OAuthException\", \"code\": 4201, "
                L"\"error_user_msg\": \"User canceled the Dialog flow\""
                L"}}";

            FBError^ err = FBError::FromJson(ErrorObjectJson);
            _dialogResponse = ref new FBResult(err);

            // deregister the event handler
            dialogWebBrowser->NavigationStarting -= navigatingEventHandlerRegistrationToken;
        }
    }

    void FacebookDialog::CloseDialogButton_OnClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
    {
        _popup->IsOpen = false;

        // If the user hits the close button, exit the dialog
		if (login_evt)
		{
			SetEvent(login_evt);
		}
	}
}
