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

#include "FacebookSession.h"

namespace Facebook
{
    ref class FBLoginButton;

    public delegate void FBLoginErrorHandler(
        FBLoginButton^ sender,
        FBError^ error
        );

    public delegate void FetchedUserInfoHandler(
        FBLoginButton^ sender,
        Facebook::Graph::FBUser^ userInfo
        );

    public delegate void ShowingLoggedInUserHandler(
        FBLoginButton^ sender
        );

    public delegate void ShowingLoggedOutUserHandler(
        FBLoginButton^ sender
        );
    
    /*!\brief Control providing a Facebook Login/Logout button
     */
    public ref class FBLoginButton sealed : Windows::UI::Xaml::Controls::Button 
    {
        public:
            FBLoginButton();

            virtual void OnApplyTemplate(
                ) override;

            ////! Login via Facebook app or Web
            //property SessionLoginBehavior LoginBehavior
            //{
            //    SessionLoginBehavior get();
            //    void set(SessionLoginBehavior);
            //}

            //! Publish permissions for user
            property Windows::Foundation::Collections::IVector<Platform::String^>^ 
                Permissions
            {
                Windows::Foundation::Collections::IVector<Platform::String^>^ get();
                void set(Windows::Foundation::Collections::IVector<Platform::String^>^);
            }

            //! Ask for read permissions at login
            void InitWithPermissions(
                Windows::Foundation::Collections::IVector<Platform::String^>^ permissions
                );

            event FBLoginErrorHandler^ FBLoginError;
            event FetchedUserInfoHandler^ FetchedUserInfo;
            event ShowingLoggedInUserHandler^ ShowingLoggedInUser;
            event ShowingLoggedOutUserHandler^ ShowingLoggedOutUser;

        private:
            Platform::String^ GetPermissions(
                );

            void OnClick(
                Platform::Object^ sender,
                Windows::UI::Xaml::RoutedEventArgs^ e
                );

//            SessionLoginBehavior m_loginBehavior;
            Platform::Collections::Vector<Platform::String^>^ m_permissions;
    };
}