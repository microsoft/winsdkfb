//******************************************************************************
//
// Copyright(c) 2016, Facebook, Inc. All rights reserved.
//
// You are hereby granted a non-exclusive, worldwide, royalty-free license to
// use, copy, modify, and distribute this software in source code or binary form
// for use in connection with the web services and APIs provided by Facebook.
//
// As with any software that integrates with the Facebook platform, your use of
// this software is subject to the Facebook Developer Principles and Policies
// [http://developers.facebook.com/policy/]. This copyright notice shall be
// included in all copies or substantial portions of the software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
//******************************************************************************

#pragma once

#ifndef __NOFBAPPEVENTS__

namespace winsdkfb
{
    public ref class FBSDKAppEvents sealed
    {
    public:
        /**
         * To integrate install tracking for mobile app install ads,
         * call this method when the app is launched.
         */
        static void ActivateApp();

        static property bool UseSimulator {
            bool get();
            void set(bool value);
        }

    private:
        /**
         * Publish an install event to the Facebook graph endpoint.
         * Write the timestamp to localSettings so we only trigger this once.
         * @param The Application Id for the request
         * @return The IAsyncAction that can be waited on for the completion
         */
        static Windows::Foundation::IAsyncAction^ PublishInstall(
            Platform::String^ AppId
            );

        /**
         * Logs a custom app event to the Facebook graph endpoint.
         * @param The Application Id for the request
         * @return The IAsyncAction that can be waited on for the completion
         */
        static Windows::Foundation::IAsyncAction^ LogActivateEvent(
            Platform::String^ AppId
            );

        /**
         * Logs an install event to the Facebook graph endpoint.
         * The user will be looked up using idfa or windows_attribution_id
         * @param The Application Id for the request
         * @return The HTTP response from the request
         */
        static Windows::Foundation::IAsyncOperation<Platform::String^>^ LogInstallEvent(
            Platform::String^ AppId
            );

        /**
         * Creates a JSON array encapsulating the activate app event
         * @return The JSON string for the event
         */
        static Platform::String^ GetActivateAppJson();

        static bool _useSimulator;
    };
}

#endif
