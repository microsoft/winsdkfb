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

#include <ppltasks.h>
#include "JsonClassFactory.h"
#include "HttpMethod.h"

namespace winsdkfb
{
    public ref class FBClient sealed
    {
    public:
        static Windows::Foundation::IAsyncOperation<Platform::String^>^ 
        GetTaskAsync(
            Platform::String^ path, 
            Windows::Foundation::Collections::PropertySet^ parameters
            );

        static Windows::Foundation::IAsyncOperation<Platform::String^>^ 
        PostTaskAsync(
            Platform::String^ path, 
            Windows::Foundation::Collections::PropertySet^ parameters
            );

        static Windows::Foundation::IAsyncOperation<Platform::String^>^ 
        DeleteTaskAsync(
            Platform::String^ path, 
            Windows::Foundation::Collections::PropertySet^ parameters
            );

        static Platform::String^ FBClient::ParametersToQueryString(
            Windows::Foundation::Collections::PropertySet^ Parameters
            );

    private:
        FBClient();
        
        static Windows::Foundation::Collections::PropertySet^ 
        GetStreamsToUpload(
            Windows::Foundation::Collections::PropertySet^ parameters
            );

        static void FBClient::AddStreamsToForm(
            Windows::Foundation::Collections::PropertySet^ Parameters,
            Windows::Web::Http::HttpMultipartFormDataContent^ Form
            );

        static Windows::Foundation::IAsyncOperation<Platform::String^>^
        SimplePostAsync(
            Platform::String^ path,
            Windows::Foundation::Collections::PropertySet^ parameters
            );

        static Windows::Foundation::IAsyncOperation<Platform::String^>^
        MultipartPostAsync(
            Platform::String^ path,
            Windows::Foundation::Collections::PropertySet^ parameters,
            Windows::Foundation::Collections::PropertySet^ streams
            );

        static Windows::Foundation::IAsyncOperation<Platform::String^>^  
        ApiAsync(
            HttpMethod httpMethod, 
            Platform::String^ path, 
            Windows::Foundation::Collections::PropertySet^ parameters, 
            Platform::Type^ resultType,
            Platform::Object^ userState,
            concurrency::cancellation_token cancellationToken
            );

        static Windows::Foundation::Collections::PropertySet^ ToDictionary(
            Windows::Foundation::Collections::PropertySet^ parameters, 
            Windows::Foundation::Collections::PropertySet^ mediaObjects, 
            Windows::Foundation::Collections::PropertySet^ mediaStreams
            );

        static Platform::String^ ParseUrlQueryString(
            Platform::String^ path, 
            Windows::Foundation::Collections::PropertySet^ parameters, 
            bool forceParseAllUrls, 
            Windows::Foundation::Uri^ uriParam
            );

        static Platform::String^ BuildHttpQuery(
            Object^ parameter
            );

        static Windows::Foundation::Uri^ PrepareRequestUri(
            winsdkfb::HttpMethod httpMethod,
            Platform::String^ path, 
            Windows::Foundation::Collections::PropertySet^ parameters, 
            Platform::Type^ resultType, 
            Windows::Storage::Streams::IRandomAccessStream^ input,
            bool& containsEtag, 
            Platform::Collections::Vector<int>^ batchEtags
            );

        static void SerializeParameters(
            Windows::Foundation::Collections::PropertySet^ parameters
            );

        static BOOL IsOAuthErrorResponse(
            Platform::String^ Response
            );

        static concurrency::task<Platform::String^> GetTaskInternalAsync(
            Windows::Foundation::Uri^ RequestUri
            );

        static concurrency::task<Platform::String^> DeleteTaskInternalAsync(
            Windows::Foundation::Uri^ RequestUri
            );

        static concurrency::task<Platform::String^> SimplePostInternalAsync(
            Windows::Foundation::Uri^ RequestUri
            );

        static concurrency::task<Platform::String^> MultipartPostInternalAsync(
            Windows::Foundation::Uri^ RequestUri,
            Windows::Foundation::Collections::PropertySet^ Streams
            );

        static concurrency::task<Platform::String^> TryReceiveHttpResponse(
            concurrency::task<Windows::Web::Http::HttpResponseMessage^> httpRequestTask,
            concurrency::cancellation_token_source cancellationTokenSource
            );
    };
};
