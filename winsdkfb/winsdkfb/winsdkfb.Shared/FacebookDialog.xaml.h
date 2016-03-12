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

namespace winsdkfb
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

        void InitDialog(
            );

        void UninitDialog(
            );

        static Platform::String^ GetFBServerUrl(
            );

        Windows::Foundation::IAsyncOperation<winsdkfb::FBResult^>^ ShowLoginDialog(
            Windows::Foundation::Collections::PropertySet^ Parameters
            );

        Windows::Foundation::IAsyncOperation<winsdkfb::FBResult^>^ ShowFeedDialog(
            Windows::Foundation::Collections::PropertySet^ Parameters
            );

        Windows::Foundation::IAsyncOperation<winsdkfb::FBResult^>^ ShowRequestsDialog(
            Windows::Foundation::Collections::PropertySet^ Parameters
            );

        Windows::Foundation::IAsyncOperation<winsdkfb::FBResult^>^ ShowSendDialog(
            Windows::Foundation::Collections::PropertySet^ Parameters
            );

        /*! discussion The current session in webview is required only if the access token is valid
         *  When the access token is removed the cookies must be clean up
         */
        static void DeleteCookies(
            );

    private:
        Windows::Foundation::IAsyncOperation<winsdkfb::FBResult^>^ ShowDialog(
            DialogUriBuilder^ uriBuilder,
            Windows::Foundation::TypedEventHandler<Windows::UI::Xaml::Controls::WebView^, Windows::UI::Xaml::Controls::WebViewNavigationStartingEventArgs^>^ EventHandler,
            Windows::Foundation::Collections::PropertySet^ Parameters
            );

        Platform::String^ GetRedirectUriString(
            Platform::String^ DialogName
            );

        static BOOL IsMobilePlatform(
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

        Windows::Foundation::Uri^ BuildSendDialogUrl(
            Windows::Foundation::Collections::PropertySet^ Parameters
            );

        void dialogWebView_LoginNavStarting(
            Windows::UI::Xaml::Controls::WebView^ sender,
            Windows::UI::Xaml::Controls::WebViewNavigationStartingEventArgs^ e
            );

        void dialogWebView_FeedNavStarting(
            Windows::UI::Xaml::Controls::WebView^ sender,
            Windows::UI::Xaml::Controls::WebViewNavigationStartingEventArgs^ e
            );

        void dialogWebView_RequestNavStarting(
            Windows::UI::Xaml::Controls::WebView^ sender,
            Windows::UI::Xaml::Controls::WebViewNavigationStartingEventArgs^ e
            );

        void dialogWebView_SendNavStarting(
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

        bool IsLogoutRedirect(
            Windows::Foundation::Uri^ Response
            );

        bool IsDialogCloseRedirect(
            Windows::Foundation::Uri^ Response
            );
        
        void OnSizeChanged(
            Windows::UI::Core::CoreWindow ^sender,
            Windows::UI::Core::WindowSizeChangedEventArgs ^args
            );

        void SetDialogResponse(winsdkfb::FBResult ^dialogResponse);

        Windows::Foundation::EventRegistrationToken
            navigatingEventHandlerRegistrationToken;
        Windows::Foundation::EventRegistrationToken
            sizeChangedEventRegistrationToken;
        Windows::UI::Xaml::Controls::Grid^ _grid;
        Windows::UI::Xaml::Controls::Primitives::Popup^ _popup;
        concurrency::task_completion_event<winsdkfb::FBResult^> _dialogResponse;
    };
}
