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

#pragma once

#include "FacebookAccessTokenData.h"
#include "FacebookResult.h"
#include "FBUser.h"
#include "FacebookDialog.xaml.h"
#include "FacebookPermissions.h"

namespace winsdkfb
{
    //! Default audience for sharing.  3 available settings
    public enum class SessionDefaultAudience
    {
        SessionDefaultAudienceNone = 0,
        SessionDefaultAudienceOnlyMe = 10,
        SessionDefaultAudienceFriends = 20,
        SessionDefaultAudienceEveryone = 30
    };

    //! Specifies behavior of login for web view vs. app
    public enum class SessionLoginBehavior
    {
        WebView,
        WebAuth,
        WebAccountProvider,
        DefaultOrdering,
        Silent
    };

    ref class FBSession;

    /*!\brief The main object for the SDK, repository for access token, etc.
     */
    public ref class FBSession sealed
    {
        public:

            //! Facebook App ID
            property Platform::String^ FBAppId
            {
                Platform::String^ get();
                void set(Platform::String^);
            }

            //! Windows App ID
            property Platform::String^ WinAppId
            {
                Platform::String^ get();
                void set(Platform::String^);
            }

            //! Response from FB App
            property Platform::String^ AppResponse 
            {
                Platform::String^ get();
            }

            //! Returns whether session (user) is logged in
            property bool LoggedIn
            {
                bool get();
            }

            //! Access token data 
            property winsdkfb::FBAccessTokenData^ AccessTokenData
            {
                winsdkfb::FBAccessTokenData^ get();
                void set(FBAccessTokenData^ value);
            }

            property int APIMajorVersion
            {
                int get();
            }

            property int APIMinorVersion
            {
                int get();
            }

            //! FBSession is a singleton object - ActiveSession is the way to
            //! acquire a reference to the object.
            static property FBSession^ ActiveSession
            {
                FBSession^ get()
                {
                    static FBSession^ activeFBSession = ref new FBSession();
                    return activeFBSession;
                }
            }

            //! Clear all login information, e.g. user info, token string, etc.
            Windows::Foundation::IAsyncAction^ LogoutAsync();

            //! User info - valid after successful login
            property winsdkfb::Graph::FBUser^ User
            {
                winsdkfb::Graph::FBUser^ get();
            }

            //! Launch 'feed' dialog, to post to user's timeline
            Windows::Foundation::IAsyncOperation<FBResult^>^ ShowFeedDialogAsync(
                Windows::Foundation::Collections::PropertySet^ Parameters
                );

            //! Launch 'request' dialog, to send app
            Windows::Foundation::IAsyncOperation<FBResult^>^ ShowRequestsDialogAsync(
                Windows::Foundation::Collections::PropertySet^ Parameters
                );

            //! Launch 'send' dialog, to send private message
            Windows::Foundation::IAsyncOperation<FBResult^>^ ShowSendDialogAsync(
                Windows::Foundation::Collections::PropertySet^ Parameters
                );

            Windows::Foundation::IAsyncOperation<FBResult^>^ LoginAsync(
                winsdkfb::FBPermissions^ Permissions
                );

            //! Login to Facebook.
            Windows::Foundation::IAsyncOperation<FBResult^>^ LoginAsync(
                winsdkfb::FBPermissions^ Permissions,
                SessionLoginBehavior behavior
                );

            void SetAPIVersion(
                int MajorVersion,
                int MinorVersion
                );

            Windows::Foundation::IAsyncOperation<FBResult^>^ TryRefreshAccessToken(
                );

        private:
            FBSession();
           
            ~FBSession();

            Windows::Foundation::Uri^ BuildLoginUri(
                Windows::Foundation::Collections::PropertySet^ Parameters
                );

            Platform::String^ GetWebAuthRedirectUriString(
                );

            concurrency::task<FBResult^> GetUserInfo(
                winsdkfb::FBAccessTokenData^ TokenData
                );

            void ParseOAuthResponse(
                Windows::Foundation::Uri^ ResponseUri
                );

            Windows::Foundation::IAsyncOperation<Windows::Storage::IStorageItem^>^ 
            MyTryGetItemAsync(
                Windows::Storage::StorageFolder^ folder,
                Platform::String^ itemName
                );

            concurrency::task<FBResult^> CheckForExistingToken(
                );

            void TrySaveTokenData(
                );

            Windows::Foundation::IAsyncAction^ TryDeleteTokenData(
                );

            concurrency::task<FBResult^> GetAppPermissions(
                );

            concurrency::task<winsdkfb::FBResult^>
                ProcessAuthResult(
                Windows::Security::Authentication::Web::WebAuthenticationResult^ authResult
                );

            concurrency::task<winsdkfb::FBResult^> TryGetUserInfoAfterLogin(
                winsdkfb::FBResult^ loginResult
                );

            concurrency::task<winsdkfb::FBResult^> TryGetAppPermissionsAfterLogin(
                winsdkfb::FBResult^ loginResult
                );

            concurrency::task<FBResult^> RunOAuthOnUiThread(
                Windows::Foundation::Collections::PropertySet^ Parameters
                );

            concurrency::task<FBResult^> RunWebViewLoginOnUIThread(
                Windows::Foundation::Collections::PropertySet^ Parameters
                );

            concurrency::task<FBResult^> ShowLoginDialog(
                Windows::Foundation::Collections::PropertySet^ Parameters
                );

            concurrency::task<FBResult^> TryLoginViaWebView(
                Windows::Foundation::Collections::PropertySet^ Parameters
                );

            concurrency::task<FBResult^> TryLoginViaWebAuthBroker(
                Windows::Foundation::Collections::PropertySet^ Parameters
                );

            concurrency::task<FBResult^> TryLoginSilently(
                Windows::Foundation::Collections::PropertySet^ Parameters
                );

            void WriteGrantedPermissionsToFile(
                );

            Platform::String^ GetGrantedPermissionsFromFile(
                );

#if defined(_WIN32_WINNT_WIN10) && (_WIN32_WINNT >= _WIN32_WINNT_WIN10)
            Platform::String^ GetWebAccountProviderRedirectUriString(
                );

            concurrency::task<FBResult^> CheckWebAccountProviderForExistingToken(
                FBPermissions^ Permissions
                );

            concurrency::task<FBResult^> TryLoginViaWebAccountProvider(
                FBPermissions^ Permissions
                );

            concurrency::task<FBResult^> CallWebAccountProviderOnUiThread(
                FBPermissions^ Permissions
                );
                
            FBResult^ ExtractAccessTokenDataFromResponseData(
                Windows::Foundation::Collections::IVectorView
                    <Windows::Security::Authentication::Web::Core::WebTokenResponse^>^ ResponseData
                );

            FBResult^ FBResultFromTokenRequestResult(
                Windows::Security::Authentication::Web::Core::WebTokenRequestResult^ RequestResult
                );
#endif

#if WINAPI_FAMILY==WINAPI_FAMILY_PHONE_APP
            Windows::Foundation::Uri^ RemoveJSONFromBrowserResponseUri(
                Windows::Foundation::Uri^ responseUri
                );
#endif

            BOOL IsRerequest(
                Windows::Foundation::Collections::PropertySet^ Parameters
                );

            Platform::String^ _FBAppId;
            Platform::String^ _WinAppId;
            bool _loggedIn;
            Platform::String^ _AppResponse;
            winsdkfb::FBAccessTokenData^ _AccessTokenData;
            winsdkfb::Graph::FBUser^ _user;
            concurrency::task<winsdkfb::FBResult^> _loginTask;
            winsdkfb::FacebookDialog^ _dialog;
            int _APIMajorVersion;
            int _APIMinorVersion;
    };
}
