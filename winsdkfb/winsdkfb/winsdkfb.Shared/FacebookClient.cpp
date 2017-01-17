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

#include "pch.h"

#include "FacebookClient.h"
#include "FacebookMediaObject.h"
#include "FacebookMediaStream.h"
#include "FacebookSession.h"
#include "HttpMethod.h"
#include "JsonClassFactory.h"
#include "SDKMessage.h"
#include "GraphUriBuilder.h"

using namespace concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace winsdkfb;
using namespace winsdkfb::Graph;
using namespace std;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage::Streams;
using namespace Windows::System::Threading;
using namespace Windows::Data::Json;
using namespace Windows::Web::Http;
using namespace Windows::Web::Http::Filters;
using namespace Windows::Web::Http::Headers;

#define BufferSize (1024 * 4) // 4kb
#define AttachmentMustHavePropertiesSetError "Attachment (FBMediaObject/FBMediaStream) must have a content type, file name, and value set."
#define AttachmentValueIsNull "The value of attachment (FBMediaObject/FBMediaStream) is null."
#define UnknownResponse = "Unknown facebook response."
#define ETagKey "_etag_"
#define MultiPartFormPrefix "--"
#define MultiPartNewLine "\r\n"
#define MultiPartContentType L"Content-Type: multipart/form-data; "
#define MultiPartBoundary L"------------------------------fbsdk1234567890"
#define UserAgent L"User-Agent"
#define WinSDKFBUserAgent(version) L"FBWinSDK." version
#define WinSDKFBUserAgentString WinSDKFBUserAgent(WINSDKFB_VERSION)

FBClient::FBClient()
{
}

// TODO: Make sure the syntax for PropertySet can be used for out parameters
PropertySet^ FBClient::ToDictionary(PropertySet^ parameters, PropertySet^ mediaObjects, PropertySet^ mediaStreams)
{
    if (parameters == nullptr)
    {
        return nullptr;
    }

    // Create a PropertySet to hold all objects that are not a MediaStream or mediaObject
    PropertySet^ dictionary = ref new PropertySet();

    // Enumerate through all the parameters
    auto first = parameters->First();
    while (first->HasCurrent)
    {
        String^ key = first->Current->Key;

        if (dynamic_cast<FBMediaObject^>(first->Current->Value))
        {
            mediaObjects->Insert(key, first->Current->Value);
        }
        else if (dynamic_cast<FBMediaStream^>(first->Current->Value))
        {
            mediaStreams->Insert(key, first->Current->Value);
        }
        else
        {
            // Add to dictionary
            dictionary->Insert(key, first->Current->Value);
        }
        //IPropertyValue^ value = safe_cast<IPropertyValue^>(first->Current->Value);
        //PropertyType valueType = value->Type;
        first->MoveNext();
    }

    return dictionary;
}

IAsyncOperation<String^>^ FBClient::GetTaskAsync(
    String^ path,
    IMapView<String^, Object^>^ parameters
    )
{
    PropertySet^ modifiableParams = MapViewToPropertySet(parameters);
    IAsyncOperation<String^>^ myTask = create_async([=]()
    {
        Uri^ uri = FBClient::PrepareRequestUri(HttpMethod::Get, path,
            modifiableParams);

        return FBClient::GetTaskInternalAsync(uri)
            .then([=](String^ Response)
        {
            task<String^> result;

            if (FBClient::IsOAuthErrorResponse(Response))
            {
                result = create_task([=]()
                {
                    FBSession^ sess = FBSession::ActiveSession;
                    return FBSession::ActiveSession->TryRefreshAccessToken();
                })
                .then([=](FBResult^ Result)
                {
                    return FBClient::GetTaskInternalAsync(uri);
                });
            }
            else
            {
                result = create_task([=]()
                {
                    return Response;
                });
            }

            return result;
        });
    });

    return myTask;
}

task<String^> FBClient::GetTaskInternalAsync(
    Uri^ RequestUri
    )
{
    HttpBaseProtocolFilter^ filter = ref new HttpBaseProtocolFilter();
    HttpClient^ httpClient = ref new HttpClient(filter);
    httpClient->DefaultRequestHeaders->Append(UserAgent, WinSDKFBUserAgentString);
    cancellation_token_source cancellationTokenSource =
        cancellation_token_source();

    filter->CacheControl->ReadBehavior = HttpCacheReadBehavior::Default;

    task<HttpResponseMessage^> httpRequestTask = create_task(httpClient->GetAsync(RequestUri), cancellationTokenSource.get_token());
    return TryReceiveHttpResponse(httpRequestTask, cancellationTokenSource);
}

PropertySet^ FBClient::GetStreamsToUpload(
    PropertySet^ parameters
    )
{
    PropertySet^ streams = nullptr;

    // Enumerate through all the parameters
    IIterator<IKeyValuePair<String^, Object^>^>^ first = parameters->First();
    while (first && (first->HasCurrent))
    {
        if (dynamic_cast<FBMediaStream^>(first->Current->Value))
        {
            if (!streams)
            {
                streams = ref new PropertySet();
            }
            streams->Insert(first->Current->Key, first->Current->Value);
            break;
        }

        first->MoveNext();
    }

    return streams;
}

IAsyncOperation<String^>^ FBClient::SimplePostAsync(
    String^ path,
    PropertySet^ parameters
    )
{
    return create_async([=]()
    {
        Uri^ uri = FBClient::PrepareRequestUri(HttpMethod::Post, path,
            parameters);

        return FBClient::SimplePostInternalAsync(uri)
            .then([=](String^ Response)
        {
            task<String^> result;

            if (FBClient::IsOAuthErrorResponse(Response))
            {
                result = create_task([=]()
                {
                    FBSession^ sess = FBSession::ActiveSession;
                    return FBSession::ActiveSession->TryRefreshAccessToken();
                })
                    .then([=](FBResult^ Result)
                {
                    return FBClient::SimplePostInternalAsync(uri);
                });
            }
            else
            {
                result = create_task([=]()
                {
                    return Response;
                });
            }

            return result;
        });
    });
}

task<String^> FBClient::SimplePostInternalAsync(
    Uri^ RequestUri
    )
{
    HttpBaseProtocolFilter^ filter = ref new HttpBaseProtocolFilter();
    HttpClient^ httpClient = ref new HttpClient(filter);
    httpClient->DefaultRequestHeaders->Append(UserAgent, WinSDKFBUserAgentString);
    cancellation_token_source cancellationTokenSource =
        cancellation_token_source();

    task<HttpResponseMessage^> httpRequestTask = create_task(httpClient->PostAsync(RequestUri, ref new HttpStringContent(L"")), cancellationTokenSource.get_token());
    return TryReceiveHttpResponse(httpRequestTask, cancellationTokenSource);
}

void FBClient::AddStreamsToForm(
    PropertySet^ Streams,
    HttpMultipartFormDataContent^ Form
    )
{
    HttpStreamContent^ fileContent = nullptr;

    // Enumerate through all the parameters
    IIterator<IKeyValuePair<String^, Object^>^>^ first = Streams->First();
    while (first->HasCurrent)
    {
        String^ key = first->Current->Key;
        FBMediaStream^ stream = dynamic_cast<FBMediaStream^>(first->Current->Value);
        if (stream)
        {
            fileContent = ref new HttpStreamContent(stream->Stream);
            HttpContentHeaderCollection^ headers = fileContent->Headers;
            headers->Insert(L"Content-Type", stream->Stream->ContentType);
            Form->Add(fileContent, key, stream->FileName);
        }

        first->MoveNext();
    }
}

IAsyncOperation<String^>^ FBClient::MultipartPostAsync(
    String^ path,
    PropertySet^ parameters,
    PropertySet^ streams
    )
{
    return create_async([=]()
    {
        Uri^ uri = FBClient::PrepareRequestUri(HttpMethod::Post, path,
            parameters);

        return FBClient::MultipartPostInternalAsync(uri, streams)
            .then([=](String^ Response)
        {
            task<String^> result;

            if (FBClient::IsOAuthErrorResponse(Response))
            {
                result = create_task([=]()
                {
                    FBSession^ sess = FBSession::ActiveSession;
                    return FBSession::ActiveSession->TryRefreshAccessToken();
                })
                    .then([=](FBResult^ Result)
                {
                    return FBClient::MultipartPostInternalAsync(uri, streams);
                });
            }
            else
            {
                result = create_task([=]()
                {
                    return Response;
                });
            }

            return result;
        });
    });
}

task<String^> FBClient::MultipartPostInternalAsync(
    Uri^ RequestUri,
    PropertySet^ Streams
    )
{
    HttpClient^ httpClient = ref new HttpClient();
    httpClient->DefaultRequestHeaders->Append(UserAgent, WinSDKFBUserAgentString);
    HttpMultipartFormDataContent^ form =
        ref new HttpMultipartFormDataContent();
    cancellation_token_source cancellationTokenSource =
        cancellation_token_source();

    FBClient::AddStreamsToForm(Streams, form);

    task<HttpResponseMessage^> httpRequestTask = create_task(httpClient->PostAsync(RequestUri, form), cancellationTokenSource.get_token());
    return TryReceiveHttpResponse(httpRequestTask, cancellationTokenSource);
}

IAsyncOperation<String^>^ FBClient::PostTaskAsync(
    String^ path,
    IMapView<String^, Object^>^ parameters
    )
{
    PropertySet^ modifiableParams = MapViewToPropertySet(parameters);
    IAsyncOperation<String^>^ result = nullptr;
    PropertySet^ streams = GetStreamsToUpload(modifiableParams);
    if (streams)
    {
        result = FBClient::MultipartPostAsync(path, modifiableParams, streams);
    }
    else
    {
        result = FBClient::SimplePostAsync(path, modifiableParams);
    }
    return result;
}

Windows::Foundation::IAsyncOperation<String^>^ FBClient::DeleteTaskAsync(
    String^ path,
    IMapView<String^, Object^>^ parameters
    )
{
    PropertySet^ modifiableParams = MapViewToPropertySet(parameters);
    return create_async([=]()
    {
        Uri^ uri = FBClient::PrepareRequestUri(HttpMethod::Delete, path,
            modifiableParams);

        return FBClient::DeleteTaskInternalAsync(uri)
            .then([=](String^ Response)
        {
            task<String^> result;

            if (FBClient::IsOAuthErrorResponse(Response))
            {
                result = create_task([=]()
                {
                    FBSession^ sess = FBSession::ActiveSession;
                    return FBSession::ActiveSession->TryRefreshAccessToken();
                })
                    .then([=](FBResult^ Result)
                {
                    return FBClient::DeleteTaskInternalAsync(uri);
                });
            }
            else
            {
                result = create_task([=]()
                {
                    return Response;
                });
            }

            return result;
        });
    });
}

task<String^> FBClient::DeleteTaskInternalAsync(
    Uri^ RequestUri
    )
{
    HttpBaseProtocolFilter^ filter = ref new HttpBaseProtocolFilter();
    HttpClient^ httpClient = ref new HttpClient(filter);
    httpClient->DefaultRequestHeaders->Append(UserAgent, WinSDKFBUserAgentString);
    cancellation_token_source cancellationTokenSource =
        cancellation_token_source();

    task<HttpResponseMessage^> httpRequestTask = create_task(httpClient->DeleteAsync(RequestUri), cancellationTokenSource.get_token());
    return TryReceiveHttpResponse(httpRequestTask, cancellationTokenSource);
}

Uri^ FBClient::PrepareRequestUri(
    winsdkfb::HttpMethod httpMethod,
    String^ path,
    PropertySet^ parameters
    )
{
    FBSession^ sess = FBSession::ActiveSession;
    GraphUriBuilder^ uriBuilder = ref new GraphUriBuilder(path);

    if (parameters == nullptr)
    {
        parameters = ref new PropertySet();
    }

    PropertySet^ mediaObjects = ref new PropertySet();
    PropertySet^ mediaStreams = ref new PropertySet();
    PropertySet^ parametersWithoutMediaObjects = ToDictionary(parameters, mediaObjects, mediaStreams);
    // ensure that media items are in valid states
    ValidateMediaStreams(mediaStreams);
    ValidateMediaObjects(mediaObjects);

    if (parametersWithoutMediaObjects == nullptr)
    {
        parametersWithoutMediaObjects = ref new PropertySet();
    }

    if (!parametersWithoutMediaObjects->HasKey("access_token") &&
        (sess->AccessTokenData != nullptr) &&
        (sess->AccessTokenData->AccessToken->Data() != nullptr) &&
        (sess->AccessTokenData->AccessToken->Length() > 0))
    {
        parametersWithoutMediaObjects->Insert("access_token",
            sess->AccessTokenData->AccessToken);
    }

    if (parametersWithoutMediaObjects->HasKey("format"))
    {
        parametersWithoutMediaObjects->Insert("format", "json-strings");
    }

    SerializeParameters(parametersWithoutMediaObjects);

    // Add remaining parameters to query string.  Note that parameters that
    // do not need to be uploaded as multipart, i.e. any which is are not
    // binary data, are required to be in the query string, even for POST
    // requests!
    auto kvp = parametersWithoutMediaObjects->First();
    while (kvp->HasCurrent)
    {
        uriBuilder->AddQueryParam(kvp->Current->Key, static_cast<String^>(kvp->Current->Value));
        kvp->MoveNext();
    }

    return uriBuilder->MakeUri();
}

void FBClient::ValidateMediaStreams(PropertySet^ mediaStreams)
{
    if (mediaStreams->Size > 0)
    {
        IIterator<IKeyValuePair<String^, Object^>^>^ facebookMediaStream = mediaStreams->First();
        while(facebookMediaStream->HasCurrent)
        {
            FBMediaStream^ mediaStream = dynamic_cast<FBMediaStream^>(facebookMediaStream->Current->Value);
            if ((mediaStream->Stream == nullptr) ||
                (mediaStream->Stream->ContentType == nullptr) ||
                (mediaStream->FileName == nullptr) ||
                (mediaStream->FileName->Length() == 0))
            {
                throw ref new InvalidArgumentException(AttachmentMustHavePropertiesSetError);
            }

            IRandomAccessStream^ stream = mediaStream->Stream;
            if (stream == nullptr)
            {
                throw ref new InvalidArgumentException(AttachmentValueIsNull);
            }
            facebookMediaStream->MoveNext();
        }
    }
}

void FBClient::ValidateMediaObjects(PropertySet^ mediaObjects)
{
    if (mediaObjects->Size > 0)
    {
        IIterator<IKeyValuePair<String^, Object^>^>^ facebookMediaObject = mediaObjects->First();
        while (facebookMediaObject->HasCurrent)
        {
            FBMediaObject^ mediaObject = dynamic_cast<FBMediaObject^>(facebookMediaObject->Current->Value);
            if ((mediaObject->GetValue()== nullptr) ||
                (mediaObject->ContentType == nullptr) ||
                (mediaObject->FileName == nullptr) ||
                (mediaObject->FileName->Length() == 0))
            {
                throw ref new InvalidArgumentException(AttachmentMustHavePropertiesSetError);
            }

            if (mediaObject->GetValue() == nullptr)
            {
                throw ref new InvalidArgumentException(AttachmentValueIsNull);
            }
            facebookMediaObject->MoveNext();
        }
    }
}

void FBClient::SerializeParameters(
    PropertySet^ parameters
    )
{
    auto keysThatAreNotString = ref new Vector<String^>();

    auto kvp = parameters->First();
    while (kvp->HasCurrent)
    {
        // Check if the value of this property set key is NOT a string
        if (dynamic_cast<String^>(kvp->Current->Value) == nullptr)
        {
            keysThatAreNotString->Append(kvp->Current->Key);
        }

        kvp->MoveNext();
    }

    auto item = keysThatAreNotString->First();
    while (item->HasCurrent)
    {
        Object^ val = parameters->Lookup(item->Current);
        String^ newValue = dynamic_cast<String^>(val);
        if (!newValue)
        {
            newValue = val->ToString();
        }

        // Replace the existing object with the new Jsonized value
        parameters->Remove(item->Current);
        parameters->Insert(item->Current, newValue);

        // Move to next
        item->MoveNext();
    }
}

String^ FBClient::ParametersToQueryString(
    IMapView<String^, Object^>^ parameters
    )
{
    String^ queryString = L"";

    // Add remaining parameters to query string.  Note that parameters that
    // do not need to be uploaded as multipart, i.e. any which is are not
    // binary data, are required to be in the query string, even for POST
    // requests!
    IIterator<IKeyValuePair<String^, Object^>^>^ kvp = parameters->First();
    while (kvp->HasCurrent)
    {
        String^ key = Uri::EscapeComponent(kvp->Current->Key);
        String^ value = Uri::EscapeComponent(
            dynamic_cast<String^>(kvp->Current->Value));

        if (queryString->Length() > 0)
        {
            queryString += "&";
        }

        queryString += key + L"=" + value;

        kvp->MoveNext();
    }

    return queryString;
}

BOOL FBClient::IsOAuthErrorResponse(
    String^ Response
    )
{
    FBError^ err = FBError::FromJson(Response);

    return (err && err->Code == 190);
}

task<String^> FBClient::TryReceiveHttpResponse(
    task<HttpResponseMessage^> httpRequestTask,
    cancellation_token_source cancellationTokenSource
    )
{
    task<String^> getHttpTask = create_task([=]()
    {
        task<String^> resultTask = create_task([]() -> String^ {return nullptr; });
        try
        {
            HttpResponseMessage^ responseMessage = httpRequestTask.get();
            if (responseMessage && responseMessage->IsSuccessStatusCode)
            {
                resultTask = create_task(responseMessage->Content->ReadAsStringAsync(), cancellationTokenSource.get_token());
            }
        }
        catch (COMException^ e)
        {
            OutputDebugString(e->ToString()->Data());
        }
        catch (const task_canceled&)
        {
            OutputDebugString(L"http request task canceled");
        }
        return resultTask;
    });
    return create_task([=]()
    {
        String^ result = nullptr;
        try
        {
            result = getHttpTask.get();
        }
        catch (COMException^ e)
        {
            OutputDebugString(e->ToString()->Data());
        }
        catch (const task_canceled&)
        {
            OutputDebugString(L"http request task canceled");
        }
        return result;
    });
}

PropertySet^ FBClient::MapViewToPropertySet(IMapView<String^, Object^>^ mapView)
{
    PropertySet^ propertySet = ref new PropertySet();
    IIterator<IKeyValuePair<String^, Object^>^>^ it = mapView->First();
    while (it->HasCurrent)
    {
        IKeyValuePair<String^, Object^>^ current = it->Current;
        propertySet->Insert(current->Key, current->Value);
        it->MoveNext();
    }
    return propertySet;
}