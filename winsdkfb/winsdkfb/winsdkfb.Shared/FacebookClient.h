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
#include "IHttpClient.h"

namespace winsdkfb
{
    public ref class FBClient sealed : public IHttpClient
    {
    public:
        FBClient();

        virtual Windows::Foundation::IAsyncOperation<Platform::String^>^ 
        GetTaskAsync(
            Platform::String^ path, 
            Windows::Foundation::Collections::PropertySet^ parameters
            );

        virtual Windows::Foundation::IAsyncOperation<Platform::String^>^ 
        PostTaskAsync(
            Platform::String^ path, 
            Windows::Foundation::Collections::PropertySet^ parameters
            );

        virtual Windows::Foundation::IAsyncOperation<Platform::String^>^ 
        DeleteTaskAsync(
            Platform::String^ path, 
            Windows::Foundation::Collections::PropertySet^ parameters
            );

        virtual Platform::String^ ParametersToQueryString(
            Windows::Foundation::Collections::PropertySet^ Parameters
            );

    private:
        
        Windows::Foundation::Collections::PropertySet^ 
        GetStreamsToUpload(
            Windows::Foundation::Collections::PropertySet^ parameters
            );

        void FBClient::AddStreamsToForm(
            Windows::Foundation::Collections::PropertySet^ Parameters,
            Windows::Web::Http::HttpMultipartFormDataContent^ Form
            );

        Windows::Foundation::IAsyncOperation<Platform::String^>^
        SimplePostAsync(
            Platform::String^ path,
            Windows::Foundation::Collections::PropertySet^ parameters
            );

        Windows::Foundation::IAsyncOperation<Platform::String^>^
        MultipartPostAsync(
            Platform::String^ path,
            Windows::Foundation::Collections::PropertySet^ parameters,
            Windows::Foundation::Collections::PropertySet^ streams
            );

        Windows::Foundation::IAsyncOperation<Platform::String^>^  
        ApiAsync(
            HttpMethod httpMethod, 
            Platform::String^ path, 
            Windows::Foundation::Collections::PropertySet^ parameters, 
            Platform::Type^ resultType,
            Platform::Object^ userState,
            concurrency::cancellation_token cancellationToken
            );

        Windows::Foundation::Collections::PropertySet^ ToDictionary(
            Windows::Foundation::Collections::PropertySet^ parameters, 
            Windows::Foundation::Collections::PropertySet^ mediaObjects, 
            Windows::Foundation::Collections::PropertySet^ mediaStreams
            );

        Platform::String^ ParseUrlQueryString(
            Platform::String^ path, 
            Windows::Foundation::Collections::PropertySet^ parameters, 
            bool forceParseAllUrls, 
            Windows::Foundation::Uri^ uriParam
            );

        Platform::String^ BuildHttpQuery(
            Object^ parameter
            );

        Windows::Foundation::Uri^ PrepareRequestUri(
            winsdkfb::HttpMethod httpMethod,
            Platform::String^ path, 
            Windows::Foundation::Collections::PropertySet^ parameters, 
            Platform::Type^ resultType, 
            Windows::Storage::Streams::IRandomAccessStream^ input,
            bool& containsEtag, 
            Platform::Collections::Vector<int>^ batchEtags
            );

        void SerializeParameters(
            Windows::Foundation::Collections::PropertySet^ parameters
            );

        BOOL IsOAuthErrorResponse(
            Platform::String^ Response
            );

        concurrency::task<Platform::String^> GetTaskInternalAsync(
            Windows::Foundation::Uri^ RequestUri
            );

        concurrency::task<Platform::String^> DeleteTaskInternalAsync(
            Windows::Foundation::Uri^ RequestUri
            );

        concurrency::task<Platform::String^> SimplePostInternalAsync(
            Windows::Foundation::Uri^ RequestUri
            );

        concurrency::task<Platform::String^> MultipartPostInternalAsync(
            Windows::Foundation::Uri^ RequestUri,
            Windows::Foundation::Collections::PropertySet^ Streams
            );
    };
};
