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

namespace Facebook
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
        SessionLoginBehaviorWithFallbackToWebView = 0,
        SessionLoginBehaviorWithNoFallbackToWebView = 1,
        SessionLoginBehaviorForcingWebView = 2
    };

    //! Specifies method for login
    public enum class SessionLoginMethod
    {
        SessionLoginMethodDefault = 0,
        SessionLoginMethodWebView = 1,
        SessionLoginMethodWebAuthBroker = 2
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
            property Facebook::FBAccessTokenData^ AccessTokenData
            {
                Facebook::FBAccessTokenData^ get();
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
            property Facebook::Graph::FBUser^ User
            {
                Facebook::Graph::FBUser^ get();
            }

            //! Launch 'feed' dialog, to post to user's timeline
            Windows::Foundation::IAsyncOperation<FBResult^>^ ShowFeedDialogAsync(
                Windows::Foundation::Collections::PropertySet^ Parameters
                );

            //! Launch 'request' dialog, to send app
            Windows::Foundation::IAsyncOperation<FBResult^>^ ShowRequestsDialogAsync(
                Windows::Foundation::Collections::PropertySet^ Parameters
                );

            Windows::Foundation::IAsyncOperation<FBResult^>^ LoginAsync(
                Facebook::FBPermissions^ Permissions,
                SessionLoginMethod method = SessionLoginMethodDefault
                );

            void SetAPIVersion(
                int MajorVersion,
                int MinorVersion
                );

        private:
            FBSession();
           
            ~FBSession();

            Windows::Foundation::Uri^ BuildLoginUri(
                Windows::Foundation::Collections::PropertySet^ Parameters
                );

            Platform::String^ GetRedirectUriString(
                );

            concurrency::task<FBResult^> GetUserInfo(
                Facebook::FBAccessTokenData^ TokenData
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

            concurrency::task<Facebook::FBResult^>
                ProcessAuthResult(
                Windows::Security::Authentication::Web::WebAuthenticationResult^ authResult
                );

            concurrency::task<Facebook::FBResult^> TryGetUserInfoAfterLogin(
                Facebook::FBResult^ loginResult
                );

            concurrency::task<Facebook::FBResult^> TryGetAppPermissionsAfterLogin(
                Facebook::FBResult^ loginResult
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

            BOOL IsRerequest(
                Windows::Foundation::Collections::PropertySet^ Parameters
                );

            Platform::String^ _FBAppId;
            Platform::String^ _WinAppId;
            bool _loggedIn;
            Platform::String^ _AppResponse;
            Facebook::FBAccessTokenData^ _AccessTokenData;
            Facebook::Graph::FBUser^ _user;
            concurrency::task<Facebook::FBResult^> _loginTask;
            Facebook::FacebookDialog^ _dialog;
            BOOL _showingDialog;
            int _APIMajorVersion;
            int _APIMinorVersion;
    };
}
