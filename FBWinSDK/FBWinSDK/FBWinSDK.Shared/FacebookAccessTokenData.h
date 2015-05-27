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

namespace Facebook
{
    /*!\brief How user was/will be logged in.  Preferred method should be app, 
     * with web as a backup in case the Facebook app isn't installed.
     */
    public enum class SessionLoginType 
    {
        LoginTypeApp, 
        LoginTypeWeb  
    };

    /*!\brief How to crop profile picture.  Facebook offers two settings,
     * square crop or center entire image in a square.
     */
    public enum class ProfilePictureCropping 
    { 
        ProfilePictureCroppingSquare, 
        ProfilePictureCroppingOriginal 
    };
   
    /*!\brief Represents an access token used for Facebook login, with 
     * associated data.
     */
    public ref class FBAccessTokenData sealed
    {
        public:
            static FBAccessTokenData^ FromUri(
                Windows::Foundation::Uri^ Response
                );

            FBAccessTokenData::FBAccessTokenData(
                Platform::String^ AccessToken,
                Windows::Foundation::DateTime Expiration,
                Platform::String^ State
                );

            //! Returns a string representation of the token
            property Platform::String^ AccessToken
            {
                Platform::String^ get();
            }

            //! Returns the app ID string
            property Platform::String^ AppID
            {
                Platform::String^ get();
            }

            //! Returns token expiration date as Windows::Foundation::DateTime
            property Windows::Foundation::DateTime ExpirationDate
            {
                Windows::Foundation::DateTime get();
            }

            //! Returns the list of permissions
            property 
                Windows::Foundation::Collections::IMapView<Platform::String^, 
                Platform::String^>^ Permissions
            {
                Windows::Foundation::Collections::IMapView<Platform::String^,
                    Platform::String^>^ get();
            }

            //! Returns user ID for the token, if available.
            property Platform::String^ UserID
            {
                Platform::String^ get();
            }

            bool IsExpired(
                );

            void AddPermissions(
                Windows::Foundation::Collections::IVectorView<Object^>^ perms
                );

        private:
            FBAccessTokenData(
                Platform::String^ AccessToken,
                Platform::String^ Expiration,
                Platform::String^ State
                );

            void CalculateExpirationDateTime(
                Platform::String^ Expiration
                );

            static Windows::Foundation::WwwFormUrlDecoder^ 
            ParametersFromResponse(
                Windows::Foundation::Uri^ Response
                );

#ifdef _DEBUG
            void DebugPrintExpirationTime(
                );
#endif

            Platform::String^ m_accessToken;
            Platform::String^ m_appId;
            Windows::Foundation::DateTime m_expirationDate;
            Platform::Collections::Map<Platform::String^, Platform::String^>^ 
                m_permissions;
            Platform::String^ m_userId;
    };
}
