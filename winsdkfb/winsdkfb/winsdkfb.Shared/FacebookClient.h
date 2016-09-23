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
    /**
     * @brief static class used to perform HTTP requests.
     */
    public ref class FBClient sealed : public IHttpClient
    {
    public:
        FBClient();

        virtual Windows::Foundation::IAsyncOperation<Platform::String^>^
        /**
         * Performs an HTTP GET request to path with parameters as the request
         * query string
         * @param path The URL to send the request to.
         * @param parameters The query string parameters of the request.
         * @return The HTTP response content
         * @exception Exception thrown on any error in the request process.
         */
        GetTaskAsync(
            Platform::String^ path,
            Windows::Foundation::Collections::PropertySet^ parameters
            );

        /**
         * Performs an HTTP POST request to path with parameters as the request
         * query string
         * @param path The URL to send the request to.
         * @param parameters The query string parameters of the request.
         * @return The HTTP response content
         * @exception Exception thrown on any error in the request process.
         */
        virtual Windows::Foundation::IAsyncOperation<Platform::String^>^
        PostTaskAsync(
            Platform::String^ path,
            Windows::Foundation::Collections::PropertySet^ parameters
            );

        /**
         * Performs an HTTP DELETE request to path with parameters as the request
         * query string
         * @param path The URL to send the request to.
         * @param parameters The query string parameters of the request.
         * @return The HTTP response content
         * @exception Exception thrown on any error in the request process.
         */
        virtual Windows::Foundation::IAsyncOperation<Platform::String^>^
        DeleteTaskAsync(
            Platform::String^ path,
            Windows::Foundation::Collections::PropertySet^ parameters
            );

        /**
         * Transforms Parameters to a query string.
         * @param Parameters The key-value pairs of the query string
         * @return A query string of the form
         * "key1=value1&key2=value2&..." etc.
         */
        virtual Platform::String^ ParametersToQueryString(
            Windows::Foundation::Collections::PropertySet^ Parameters
            );

    private:

        /**
         * Finds all FBMediaStream object in parameters.
         * @param parameters The PropertySet to search for FBMediaStream objects in
         * @return PropertySet containing all FBMediaStream objects found. If
         * none are found, nullptr is instead returned.
         */
        Windows::Foundation::Collections::PropertySet^
        GetStreamsToUpload(
            Windows::Foundation::Collections::PropertySet^ parameters
            );

        /**
         * Adds FBMediaStream objects to Form.
         * @param Parameters PropertySet that contains FBMediaStream objects to
         * attach to Form. Will skip any values in Parameters that cannot be
         * cast to FBMediaStream.
         * @param Form The form to attach FBMediaStream objects to.
         */
        void FBClient::AddStreamsToForm(
            Windows::Foundation::Collections::PropertySet^ Parameters,
            Windows::Web::Http::HttpMultipartFormDataContent^ Form
            );

        /**
         * Non-stream HTTP POST flow.
         * @param path URL to send POST request to
         * @param parameters query parameters to attach to POST request
         * @return IAsyncOperation containing the response content
         * @exception Can throw any exception that is thrown by SimplePlostInternalAsync
         */
        Windows::Foundation::IAsyncOperation<Platform::String^>^
        SimplePostAsync(
            Platform::String^ path,
            Windows::Foundation::Collections::PropertySet^ parameters
            );

        /**
         * HTTP POST request flow with streams.
         * @param path URL to send POST request to
         * @param parameters query parameters to attach to POST request
         * @param streams streams to attach to POST request
         * @return IASyncOperation containing the response content
         * @exception Can throw any exception that is thrown by MultipartPostInternalAsync
         */
        Windows::Foundation::IAsyncOperation<Platform::String^>^
        MultipartPostAsync(
            Platform::String^ path,
            Windows::Foundation::Collections::PropertySet^ parameters,
            Windows::Foundation::Collections::PropertySet^ streams
            );

        /**
         * Sorts parameters into FBMediaStream, FBMediaObject, and everything else.
         * @param parameters The PropertySet to sort
         * @param mediaObjects PropertySet to append FBmediaObject objects to
         * @param mediaStreams PropertySet to append FBmediaStream objects to
         * @return PropertySet of all objects that are not a FBMediaStrem or a
         * FBMediaObject. If parameters is nullptr, will instead return nullptr.
         * Note that mediaObjects and mediaStreams are both altered by this function.
         */
        static Windows::Foundation::Collections::PropertySet^ ToDictionary(
            Windows::Foundation::Collections::PropertySet^ parameters,
            Windows::Foundation::Collections::PropertySet^ mediaObjects,
            Windows::Foundation::Collections::PropertySet^ mediaStreams
            );

        /**
         * Builds request URI.
         * @param httpMethod Type of HTTP request to build URI for
         * @param path Request path
         * @param parameters Query parameters for the request
         * @param input TODO
         * @return Request URI
         * @exception FailureExecption if FBMediaObject or FBMediaStream are
         * attempting to be attached on non-POST requests.
         * @exception InvalidArgumentException if httpMethod is POST and improperly formatted/empty media object is attached.
         */
        Windows::Foundation::Uri^ PrepareRequestUri(
            winsdkfb::HttpMethod httpMethod,
            Platform::String^ path,
            Windows::Foundation::Collections::PropertySet^ parameters,
            Windows::Storage::Streams::IRandomAccessStream^ input
            );

        /**
         * Serializes non-string values in parameters to JSON strings. Note that
         * this function modifies parameters.
         * @param parameters The PropertySet to modify
         */
        void SerializeParameters(
            Windows::Foundation::Collections::PropertySet^ parameters
            );

        /**
         * Checks if Response is an OAuth error response.
         * @param Response response to check
         * @return true if Response does indicate an OAuth error, false otherwise.
         */
        BOOL IsOAuthErrorResponse(
            Platform::String^ Response
            );

        /**
         * Performs the actual HTTP GET request.
         * @param RequestUri the full URI of the request
         * @return The response content
         * @exception Exception Any exception that can occur during the request
         */
        concurrency::task<Platform::String^> GetTaskInternalAsync(
            Windows::Foundation::Uri^ RequestUri
            );

        /**
         * Performs the actual HTTP DELETE request.
         * @param RequestUri the full URI of the request
         * @return The response content
         * @exception Exception Any exception that can occur during the request
         */
        concurrency::task<Platform::String^> DeleteTaskInternalAsync(
            Windows::Foundation::Uri^ RequestUri
            );

        /**
         * Performs the actual HTTP POST request with no POST data.
         * @param RequestUri the full URI of the request
         * @return The response content
         * @exception Exception Any exception that can occur during the request
         */
        concurrency::task<Platform::String^> SimplePostInternalAsync(
            Windows::Foundation::Uri^ RequestUri
            );

        /**
         * Performs the actual HTTP POST request with POST stream data.
         * @param RequestUri the full URI of the request
         * @return The response content
         * @exception Exception Any exception that can occur during the request
         */
        concurrency::task<Platform::String^> MultipartPostInternalAsync(
            Windows::Foundation::Uri^ RequestUri,
            Windows::Foundation::Collections::PropertySet^ Streams
            );

        Platform::String^ FBClient::MovePathQueryStringToParameters(
            Platform::String^ path,
            Windows::Foundation::Collections::PropertySet^ parameters
            );

        concurrency::task<Platform::String^> TryReceiveHttpResponse(
            concurrency::task<Windows::Web::Http::HttpResponseMessage^> httpRequestTask,
            concurrency::cancellation_token_source cancellationTokenSource
            );
    };
};
