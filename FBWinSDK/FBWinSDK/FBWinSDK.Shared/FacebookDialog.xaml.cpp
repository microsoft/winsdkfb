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

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Popups;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Facebook;

#if (defined(_MSC_VER) && (_MSC_VER >= 1800)) 
using namespace concurrency;
#else
using namespace pplx;
#endif

using namespace std;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236
namespace Facebook
{
    FacebookDialog::FacebookDialog()
    {
        InitializeComponent();
        _oauthResponse = nullptr;
    }

    //AccessTokenData^ FacebookDialog::AccessTokenData::get()
    //{
    //    return _accessToken;
    //}

    Uri^ FacebookDialog::OAuthResponse::get()
    {
        return _oauthResponse;
    }

    extern HANDLE login_evt;

    void FacebookDialog::ShowLoginDialog(
        Popup^ Popup
        )
    {
        Uri^ loginDialogUrl = BuildLoginDialogUrl();
        navigatingEventHandlerRegistrationToken = dialogWebBrowser->NavigationStarting +=
            ref new TypedEventHandler<WebView^, WebViewNavigationStartingEventArgs^>(
                this, &FacebookDialog::dialogWebView_NavStarting);

        String^ msg = loginDialogUrl + L"\n";
        OutputDebugString(msg->Data());

        dialogWebBrowser->Navigate(loginDialogUrl);
        _popup = Popup;
    }

    void FacebookDialog::ShowFeedDialog(
        Popup^ Popup 
        )
    {
        Uri^ feedDialogUrl = BuildFeedDialogUrl();
        navigatingEventHandlerRegistrationToken = dialogWebBrowser->NavigationStarting += 
            ref new TypedEventHandler<WebView^, WebViewNavigationStartingEventArgs^>(
                this, &FacebookDialog::dialogWebView_NavStarting);
        
        dialogWebBrowser->Navigate(feedDialogUrl);
        _popup = Popup;
    }

    void FacebookDialog::ShowRequestsDialog(
        Popup^ Popup 
        )
    {
        Uri^ requestDialogUrl = BuildRequestsDialogUrl();
        navigatingEventHandlerRegistrationToken = dialogWebBrowser->NavigationStarting +=
            ref new TypedEventHandler<WebView^, WebViewNavigationStartingEventArgs^>(
                this, &FacebookDialog::dialogWebView_NavStarting);

        dialogWebBrowser->Navigate(requestDialogUrl);
        _popup = Popup;
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
            FacebookDialogName + "_success.html&app_id=" +
            sess->FBAppId + L"&display=touch";

        return result;
    }

    Uri^ FacebookDialog::BuildLoginDialogUrl(
        )
    {
        FBSession^ sess = FBSession::ActiveSession;
        String^ dialogUriString =
            L"https://m.facebook.com/dialog/oauth?client_id=" +
            sess->FBAppId + L"&redirect_uri=" + 
            GetRedirectUriString(L"login") + L"&scope="
            + sess->PermissionsToString() + L"&display=popup" +
            L"&response_type=token";

        return ref new Uri(dialogUriString);
    }

    Uri^ FacebookDialog::BuildFeedDialogUrl(
        )
    {
        FBSession^ sess = FBSession::ActiveSession;
        String^ dialogUriString =
            L"https://m.facebook.com/v2.1/dialog/feed?access_token=" +
            sess->AccessTokenData->AccessToken +
            L"&redirect_uri=" + GetRedirectUriString(L"feed"); 

        return ref new Uri(dialogUriString);
    }

    Uri^ FacebookDialog::BuildRequestsDialogUrl(
        )
    {
        FBSession^ sess = FBSession::ActiveSession;
        //TODO: YOUR_MESSAGE_HERE?  Do we need a message parameter here?  It 
        //doesn't appear to show up anywhere in the requests dialog...
        String^ dialogUriString =
            L"https://m.facebook.com/v2.1/dialog/apprequests?access_token=" +
            sess->AccessTokenData->AccessToken +
            L"&redirect_uri=" + GetRedirectUriString(L"requests") +
            L"app_id=" + sess->FBAppId + L"&message=YOUR_MESSAGE_HERE&display=touch";

        return ref new Uri(dialogUriString);
    }

    bool FacebookDialog::IsLoginSuccessRedirect(
        Uri^ Response
        )
    {
        return (String::CompareOrdinal(Response->Path, L"/connect/login_success.html") == 0);
    }

    void FacebookDialog::dialogWebView_NavStarting(
        WebView^ sender, 
        WebViewNavigationStartingEventArgs^ e
        )
    {
        String^ msg = e->Uri->Path + L"\n";
        OutputDebugString(msg->Data());

        msg = e->Uri->DisplayUri + L"\n";
        OutputDebugString(msg->Data());

        if (IsLoginSuccessRedirect(e->Uri))
        {
            //TODO: Figure out why we're never actually navigating to the 
            //redirect Uri.  What I get now instead is, first an event for
            //the Uri I sent, then an event for the Uri without any query
            //string.
            dialogWebBrowser->Stop();
            
            _popup->IsOpen = false;

            FBAccessTokenData^ tokenData = FBAccessTokenData::FromUri(e->Uri);
            FBSession::ActiveSession->AccessTokenData = tokenData;

            // signal that we are done! The dialog is over
			if (login_evt)
			{
				SetEvent(login_evt);
			}

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
