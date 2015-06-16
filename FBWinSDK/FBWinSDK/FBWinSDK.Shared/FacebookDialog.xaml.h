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
#include "FacebookResult.h"

namespace Facebook
{
    delegate Windows::Foundation::Uri^ DialogUriBuilder(
        Windows::Foundation::Collections::PropertySet^ Parameters
        );

	[Windows::Foundation::Metadata::WebHostHidden]
    public ref class FacebookDialog sealed
    {
    public:

        FacebookDialog(
            );

        virtual ~FacebookDialog(
            );

        property Facebook::FBResult^ DialogResponse
        {
            Facebook::FBResult^ get();
        }

        void FacebookDialog::ShowLoginDialog(
            );

        void FacebookDialog::ShowFeedDialog(
            Windows::Foundation::Collections::PropertySet^ Parameters
            );

        void ShowRequestsDialog(
            Windows::Foundation::Collections::PropertySet^ Parameters
            );

    private:
        void FacebookDialog::ShowDialog(
            DialogUriBuilder^ uriBuilder,
            Windows::Foundation::TypedEventHandler<Windows::UI::Xaml::Controls::WebView^, Windows::UI::Xaml::Controls::WebViewNavigationStartingEventArgs^>^ EventHandler,
            Windows::Foundation::Collections::PropertySet^ Parameters
            );

        Platform::String^ GetRedirectUriString(
            Platform::String^ DialogName
            );

        BOOL FacebookDialog::IsMobilePlatform(
            );

        Platform::String^ FacebookDialog::GetFBServer(
            );

        Windows::Foundation::Uri^ BuildLoginDialogUrl(
            Windows::Foundation::Collections::PropertySet^ Parameters
            );

        Windows::Foundation::Uri^ BuildFeedDialogUrl(
            Windows::Foundation::Collections::PropertySet^ Parameters
            );

        Windows::Foundation::Uri^ BuildRequestsDialogUrl(
            Windows::Foundation::Collections::PropertySet^ Parameters
            );

        Windows::Foundation::EventRegistrationToken 
            navigatingEventHandlerRegistrationToken;
        
        void FacebookDialog::dialogWebView_LoginNavStarting(
            Windows::UI::Xaml::Controls::WebView^ sender,
            Windows::UI::Xaml::Controls::WebViewNavigationStartingEventArgs^ e
            );

        void FacebookDialog::dialogWebView_FeedNavStarting(
            Windows::UI::Xaml::Controls::WebView^ sender,
            Windows::UI::Xaml::Controls::WebViewNavigationStartingEventArgs^ e
            );

        void FacebookDialog::dialogWebView_RequestNavStarting(
            Windows::UI::Xaml::Controls::WebView^ sender,
            Windows::UI::Xaml::Controls::WebViewNavigationStartingEventArgs^ e
            );

        void CloseDialogButton_OnClick(
            Platform::Object^ sender, 
            Windows::UI::Xaml::RoutedEventArgs^ e
            );

        bool IsLoginSuccessRedirect(
            Windows::Foundation::Uri^ Response
            );

        void OnSizeChanged(
            Windows::UI::Core::CoreWindow ^sender,
            Windows::UI::Core::WindowSizeChangedEventArgs ^args
            );

        Windows::UI::Xaml::Controls::Grid^ _grid;
        Windows::UI::Xaml::Controls::Primitives::Popup^ _popup;
        Facebook::FBResult^ _dialogResponse;
    };
}
