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
// FacebookDialog.xaml.h
// Declaration of the FacebookDialog class
//

#pragma once

#include "FacebookDialog.g.h"
//#include "FBWinAccessTokenData.h"

namespace Facebook
{
	[Windows::Foundation::Metadata::WebHostHidden]
    public ref class FacebookDialog sealed
    {
    public:

        FacebookDialog(
            );

        void FacebookDialog::ShowLoginDialog(
            Windows::UI::Xaml::Controls::Primitives::Popup^ popup
            );

        void FacebookDialog::ShowFeedDialog(
            Windows::UI::Xaml::Controls::Primitives::Popup^ popup
            );

        void ShowRequestsDialog(
            Windows::UI::Xaml::Controls::Primitives::Popup^ popup
            );

        property Windows::Foundation::Uri^ OAuthResponse
        {
            Windows::Foundation::Uri^ get();
        }

    private:

        Platform::String^ GetRedirectUriString(
            Platform::String^ DialogName
            );

        Windows::Foundation::Uri^ BuildLoginDialogUrl(
            );

        Windows::Foundation::Uri^ BuildFeedDialogUrl(
            );

        Windows::Foundation::Uri^ BuildRequestsDialogUrl(
            );

        Windows::Foundation::EventRegistrationToken 
            navigatingEventHandlerRegistrationToken;
        
        void FacebookDialog::dialogWebView_NavStarting(
            Windows::UI::Xaml::Controls::WebView^ sender,
            Windows::UI::Xaml::Controls::WebViewNavigationStartingEventArgs^ e
            );

        void CloseDialogButton_OnClick(
            Platform::Object^ sender, 
            Windows::UI::Xaml::RoutedEventArgs^ e
            );

        Windows::UI::Xaml::Controls::Primitives::Popup^ _popup;
        Windows::Foundation::Uri^ _oauthResponse;
    };
}
