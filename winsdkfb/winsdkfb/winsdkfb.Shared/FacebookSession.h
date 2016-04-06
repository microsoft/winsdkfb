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
    //! Default audience for sharing.
    public enum class SessionDefaultAudience
    {
        SessionDefaultAudienceNone = 0,
        SessionDefaultAudienceOnlyMe = 10,
        SessionDefaultAudienceFriends = 20,
        SessionDefaultAudienceEveryone = 30
    };

    /**
     * Specifies behavior of LoginAsync(). Note that:
     * - WebAuth is not available on Windows Phone 8.1
     * - WebAccountProvider is available only on Windows 10
     */
    public enum class SessionLoginBehavior
    {
        WebView,
        WebAuth,
        WebAccountProvider,
        DefaultOrdering,
        Silent
    };

    ref class FBSession;

    /**
     * The main object for the SDK, repository for access token, etc.
     * FBSession is a singleton class that is used to configure the sdk's
     * interaction with Facebook.
     */
    public ref class FBSession sealed
    {
        public:

            /**
             * Facebook App ID that is provided by Facebook in their developer
             * portal.
             */
            property Platform::String^ FBAppId
            {
                Platform::String^ get();
                void set(Platform::String^);
            }

            /**
             * Windows App ID, must match the appliction's SID and the Windows
             * Store SID field in the Facebook developer page for the app.
             */
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

            /**
             * Access token data, only populated after LoginAsync() has been
             * called.
             */
            property winsdkfb::FBAccessTokenData^ AccessTokenData
            {
                winsdkfb::FBAccessTokenData^ get();
                void set(FBAccessTokenData^ value);
            }

            /**
             * The Facebook API major version that is specified with most http
             * requests to Facebook. Defaults to 2. Can be changed with
             * SetAPIVersion().
             */
            property int APIMajorVersion
            {
                int get();
            }

            /**
             * The Facebook API minor version that is specified with most http
             * requests to Facebook. Defaults to 1. Can be changed with
             * SetAPIVersion().
             */
            property int APIMinorVersion
            {
                int get();
            }

            //! User info - valid after successful login
            property winsdkfb::Graph::FBUser^ User
            {
                winsdkfb::Graph::FBUser^ get();
            }

            property SessionLoginBehavior LoginMethod
            {
                SessionLoginBehavior get();
            }

            property Platform::String^ WebAuthDialogRedirectUrl
            {
                void set(Platform::String^ url);
                Platform::String^ get();

            }

            /**
             * FBSession is a singleton object - ActiveSession is the way to
             * acquire a reference to the object.
             */
            static property FBSession^ ActiveSession
            {
                FBSession^ get()
                {
                    static FBSession^ activeFBSession = ref new FBSession();
                    return activeFBSession;
                }
            }

            /**
             * Clear all login information, e.g. user info, token string, etc.
             * Do note that this does not clear cookies.
             */
            Windows::Foundation::IAsyncAction^ LogoutAsync();


            /**
             * Launch 'feed' dialog, to post to user's timeline.
             * @param Parameters The collection of parameters used by the feed
             * dialog. See https://developers.facebook.com/docs/sharing/reference/feed-dialog
             * For more information.
             * @return FBResult indicating the result of the dialog flow.
             */
            Windows::Foundation::IAsyncOperation<FBResult^>^ ShowFeedDialogAsync(
                Windows::Foundation::Collections::PropertySet^ Parameters
                );

            /**
             *  Launch 'request' dialog, to send app requests to user's
             * Facebook friends.
             * @param Parameters The collection of parameters used by the
             * request dialog, the most notable being "message".
             * @return FBResult indicating the result of the dialog flow.
             */
            Windows::Foundation::IAsyncOperation<FBResult^>^ ShowRequestsDialogAsync(
                Windows::Foundation::Collections::PropertySet^ Parameters
                );

            /**
             * Launch 'send' dialog, to send private message to user's friends.
             * @param Parameters The collection of parameters used by the send
             * dialog. See https://developers.facebook.com/docs/sharing/reference/send-dialog
             * for more information.
             * @return FBResult indicating the result of the dialog flow.
             */
            Windows::Foundation::IAsyncOperation<FBResult^>^ ShowSendDialogAsync(
                Windows::Foundation::Collections::PropertySet^ Parameters
                );

            /**
             * Login to Facebook. This method defaults to SessionLoginBehavior::DefaultOrdering
             * for its login method.
             * @param Permissions The Facebook permissions that the app is requesting.
             * @return FBResult indicating the result of the Login attempt.
             */
            Windows::Foundation::IAsyncOperation<FBResult^>^ LoginAsync(
                winsdkfb::FBPermissions^ Permissions
                );

            /**
             * Login to Facebook. This method allows the login method to be
             * specified.
             * @param Permissions The Facebook permissions that the app is requesting.
             * @param behavior The login behavior to make the login attempt with.
             * @return FBResult indicating the result of the Login attempt.
             */
            Windows::Foundation::IAsyncOperation<FBResult^>^ LoginAsync(
                winsdkfb::FBPermissions^ Permissions,
                SessionLoginBehavior behavior
                );

            /**
             * Sets the Facebook API version that is specified with most API
             * calls to Facebook.
             * @param MajorVersion value for API major version.
             * @param MinorVersion value for API minor version.
             */
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

            void SaveGrantedPermissions(
                );

            Platform::String^ GetGrantedPermissions(
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
            SessionLoginBehavior _LoginMethod;
            Platform::String^ _webAuthDialogRedirectUrl;
    };
}
