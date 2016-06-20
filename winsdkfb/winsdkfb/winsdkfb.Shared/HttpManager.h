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

#include "IHttpClient.h"

namespace winsdkfb
{
    public ref class HttpManager sealed : public IHttpClient
    {
    public:
        static property HttpManager^ Instance
        {
            HttpManager^ get();
        }

        void SetHttpClient(
            winsdkfb::IHttpClient^ httpClient
            );

        virtual Windows::Foundation::IAsyncOperation<Platform::String^>^ GetTaskAsync(
            Platform::String^ path,
            Windows::Foundation::Collections::PropertySet^ parameters
            );

        virtual Windows::Foundation::IAsyncOperation<Platform::String^>^ PostTaskAsync(
            Platform::String^ path,
            Windows::Foundation::Collections::PropertySet^ parameters
            );

        virtual Windows::Foundation::IAsyncOperation<Platform::String^>^ DeleteTaskAsync(
            Platform::String^ path,
            Windows::Foundation::Collections::PropertySet^ parameters
            );
        
        virtual Platform::String^ ParametersToQueryString(
            Windows::Foundation::Collections::PropertySet^ Parameters
            );

    private:
        static HttpManager^ _instance;
        HttpManager(winsdkfb::IHttpClient^ httpClient);
        winsdkfb::IHttpClient^ _httpClient;
        
    };
}
