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

            //! Returns the list of permissions
            property Windows::Foundation::Collections::IVectorView<Platform::String^>^ Permissions
            {
                Windows::Foundation::Collections::IVectorView<Platform::String^>^ get();
            }

            //! Request a new permission
            void AddPermission(
                Platform::String^ permission
                );

            void ResetPermissions(
                );

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

            Platform::String^ PermissionsToString(
                );

            Windows::Foundation::IAsyncOperation<FBResult^>^ LoginAsync(
                );

			void SetAPIVersion(
				int MajorVersion,
				int MinorVersion
				);

        private:
            FBSession();
           
			~FBSession();

            Windows::Foundation::Uri^ BuildLoginUri(
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
                );

            concurrency::task<FBResult^> RunWebViewLoginOnUIThread(
                );

            concurrency::task<FBResult^> ShowLoginDialog(
                );

            concurrency::task<FBResult^> TryLoginViaWebView(
                );

            concurrency::task<FBResult^> TryLoginViaWebAuthBroker(
                );

            Platform::String^ _FBAppId;
            Platform::String^ _WinAppId;
            bool _loggedIn;
            Platform::String^ _AppResponse;
            Facebook::FBAccessTokenData^ _AccessTokenData;
            Platform::Collections::Vector<Platform::String^>^ _permissions;
            Facebook::Graph::FBUser^ _user;
			concurrency::task<Facebook::FBResult^> _loginTask;
            Facebook::FacebookDialog^ _dialog;
            BOOL _showingDialog;
			int _APIMajorVersion;
			int _APIMinorVersion;
    };
}
