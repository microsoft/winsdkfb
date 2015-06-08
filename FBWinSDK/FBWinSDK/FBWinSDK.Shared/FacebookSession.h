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
            }

            //! Returns the list of permissions
            property Windows::Foundation::Collections::IVectorView<Platform::String^>^ Permissions
            {
                Windows::Foundation::Collections::IVectorView<Platform::String^>^ get();
            }

            //! Expiration date/time of active session
            property Windows::Foundation::DateTime Expires
            {
                Windows::Foundation::DateTime get();
                void set(Windows::Foundation::DateTime);
            }

            //! Is session expired?
            property bool IsExpired
            {
                bool get();
            }

            //! Request a new permission
            void AddPermission(
                Platform::String^ permission
                );

            void ResetPermissions(
                );

            //! FBSession is a singleton object - ActiveSession is the way to
            // acquire a reference to the object.
            static property FBSession^ ActiveSession
            {
                FBSession^ get()
                {
                    static FBSession^ activeFBSession = ref new FBSession();
                    return activeFBSession;
                }
            }

            //! Clear all login information, e.g. user info, token string, etc.
            void Logout();

            //! User info - valid after successful login
            property Facebook::Graph::FBUser^ User
            {
                Facebook::Graph::FBUser^ get();
            }

            //! Launch 'feed' dialog, to post to user's timeline
            Windows::Foundation::IAsyncAction^ ShowFeedDialog(
                );

            //! Launch 'request' dialog, to send app
            Windows::Foundation::IAsyncAction^ ShowRequestsDialog(
                );

            Platform::String^ PermissionsToString(
                );

#if WINAPI_FAMILY==WINAPI_FAMILY_APP
            Windows::Foundation::IAsyncOperation<FBResult^>^ LoginAsync(
                );
#else if WINAPI_FAMILY==WINAPI_FAMILY_PHONE_APP
            //! Start process of logging in via FB app, i.e. launch a 
            // well-defined fbconnect:// URI, with proper app ID, params,
            // and redirect URI.
            Windows::Foundation::IAsyncOperation<FBResult^>^ 
            LoginAndContinue(
                );

            bool IsLoginResponse(
                Platform::String^ Response
                );

            bool IsFeedDialogResponse(
                Platform::String^ Response
                );

            bool IsRequestDialogResponse(
                Platform::String^ Response
                );

            //! Finish logging in when app is activated via custom protocol,
            // i.e. "called back" by Facebook app.  When async action 
            // completes, on success FBSession::User property will be valid.
            //
            // @param p - ProtocolActivatedEventArgs - this function should be
            // called from App::OnActivate, in the case of protocol activation.
            Windows::Foundation::IAsyncOperation<FBResult^>^ 
            ContinueAction(
                Windows::ApplicationModel::Activation::ProtocolActivatedEventArgs^ p
                );
            
            Windows::Foundation::IAsyncOperation<FBResult^>^ 
            ContinueLogin(
                Windows::Foundation::Uri^ Response 
                );

            Windows::Foundation::IAsyncOperation<FBResult^>^ 
            ContinuePostToFeed(
                Windows::Foundation::Uri^ Response 
                );

            Windows::Foundation::IAsyncOperation<FBResult^>^ 
            ContinueAppRequest(
                Windows::Foundation::Uri^ Response 
                );
#endif

        private:
            FBSession();
           
			~FBSession();

            Windows::Foundation::Uri^ BuildLoginUri(
                );

            Platform::String^ GetRedirectUriString(
                );

            concurrency::task<FBResult^> FBSession::GetUserInfo(
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

            void TryDeleteTokenData(
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
                );

            concurrency::task<FBResult^> ShowLoginDialog(
                );

            concurrency::task<FBResult^> TryLoginViaWebView(
                );

            concurrency::task<FBResult^> FBSession::TryLoginViaWebAuthBroker(
                );

#if WINAPI_FAMILY==WINAPI_FAMILY_PHONE_APP
            Windows::Foundation::Uri^ RemoveJSONFromBrowserResponseUri(
                Windows::Foundation::Uri^ responseUri
                );
#endif

            int64 FBSession::SecondsTilTokenExpires(
                Windows::Foundation::DateTime Expiration
                );

            Platform::String^ m_FBAppId;
            Platform::String^ m_WinAppId;
            bool m_loggedIn;
            Platform::String^ m_AppResponse;
            Facebook::FBAccessTokenData^ m_AccessTokenData;
            Platform::Collections::Vector<Platform::String^>^ m_permissions;
            Windows::Foundation::DateTime m_Expires;
            Facebook::Graph::FBUser^ m_user;
			concurrency::task<Facebook::FBResult^> m_loginTask;
    };
}
