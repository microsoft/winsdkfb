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

String^ FBClient::BuildHttpQuery(Object^ parameter)
{
    if (parameter == nullptr)
    {
        return "null";
    }
    if (dynamic_cast<String^>(parameter) != nullptr)
    {
        return dynamic_cast<String^>(parameter);
    }

    if (dynamic_cast<Uri^>(parameter) != nullptr)
    {
        return dynamic_cast<Uri^>(parameter)->ToString();
    }

    // TODO: Refactor string manipulation code here.  The usage of String^ is
    // convoluted and not necessary, it can be a lot simpler.
    String^ sb = ref new String();
    if (dynamic_cast<PropertySet^>(parameter) != nullptr)
    {
        PropertySet^ mediaObjects = ref new PropertySet();
        PropertySet^ mediaStreams = ref new PropertySet();
        auto dict = ToDictionary(dynamic_cast<PropertySet^>(parameter), mediaObjects, mediaStreams);

        if (mediaObjects->Size > 0 || mediaStreams->Size > 0)
        {
            throw ref new InvalidArgumentException("Parameter can contain attachements (FBMediaObject/FBMediaStream) only in the top most level.");
        }

        auto kvp = dict->First();
        while (kvp->HasCurrent)
        {
            String::Concat(sb, BuildHttpQuery(kvp->Current->Value));

            kvp->MoveNext();
        }
    }

    if (sb->Length() > 0)
    {
        wstring sbstl(sb->Data());
        sb = ref new String(sbstl.substr(0, sbstl.length() - 1).c_str());
    }

    return sb;
}

IAsyncOperation<String^>^ FBClient::GetTaskAsync(
    String^ path,
    PropertySet^ parameters
    )
{
    IAsyncOperation<String^>^ myTask = create_async([=]()
    {
        bool containsEtag = false;
        Uri^ uri = FBClient::PrepareRequestUri(HttpMethod::Get, path,
            parameters, nullptr, nullptr, containsEtag, nullptr);

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
    cancellation_token_source cancellationTokenSource =
        cancellation_token_source();

    filter->CacheControl->ReadBehavior = HttpCacheReadBehavior::Default;

    return create_task(httpClient->GetAsync(RequestUri), 
        cancellationTokenSource.get_token())
    .then([=](HttpResponseMessage^ response)
    {
        return create_task(response->Content->ReadAsStringAsync(), 
            cancellationTokenSource.get_token());
    })
    .then([=](task<String^> resultTask)
    {
        String^ result = nullptr;
        try
        {
            result = resultTask.get();
        }
        catch (const task_canceled&)
        {
        }
        catch (Exception^ ex)
        {
            throw ex;
        }

        return result;
    });
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
        bool containsEtag = false;
        Uri^ uri = FBClient::PrepareRequestUri(HttpMethod::Post, path,
            parameters, nullptr, nullptr, containsEtag, nullptr);

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
    cancellation_token_source cancellationTokenSource =
        cancellation_token_source();

    return create_task(
        httpClient->PostAsync(RequestUri, ref new HttpStringContent(L"")),
        cancellationTokenSource.get_token())
        .then([=](HttpResponseMessage^ response)
    {
        return create_task(response->Content->ReadAsStringAsync(),
            cancellationTokenSource.get_token());
    })
        .then([=](task<String^> previousTask)
    {
        String^ response = nullptr;

        try
        {
            // Check if any previous task threw an exception.
            response = previousTask.get();
        }
        catch (const task_canceled&)
        {
        }
        catch (Exception^ ex)
        {
        }

        return response;
    });
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
        bool containsEtag = false;
        Uri^ uri = FBClient::PrepareRequestUri(HttpMethod::Post, path,
            parameters, nullptr, nullptr, containsEtag, nullptr);

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
    HttpMultipartFormDataContent^ form =
        ref new HttpMultipartFormDataContent();
    cancellation_token_source cancellationTokenSource =
        cancellation_token_source();
    HttpResponseMessage^ msg = nullptr;
    String^ response = L"";

    FBClient::AddStreamsToForm(Streams, form);

    return create_task(httpClient->PostAsync(RequestUri, form),
        cancellationTokenSource.get_token())
        .then([=](HttpResponseMessage^ response) -> task<String^>
    {
        return create_task(response->Content->ReadAsStringAsync(),
            cancellationTokenSource.get_token());
    })
        .then([=](task<String^> previousTask) -> String^
    {
        String^ response = nullptr;

        try
        {
            // Check if any previous task threw an exception.
            response = previousTask.get();
        }
        catch (const task_canceled&)
        {
        }
        catch (Exception^ ex)
        {
        }

        return response;
    });
}

IAsyncOperation<String^>^ FBClient::PostTaskAsync(
    String^ path,
    PropertySet^ parameters
    )
{
    IAsyncOperation<String^>^ result = nullptr;
    PropertySet^ streams = GetStreamsToUpload(parameters);
    if (streams)
    {
        result = FBClient::MultipartPostAsync(path, parameters, streams);
    }
    else
    {
        result = FBClient::SimplePostAsync(path, parameters);
    }

    return result;
}

Windows::Foundation::IAsyncOperation<String^>^ FBClient::DeleteTaskAsync(
    String^ path, 
    PropertySet^ parameters
    )
{
    return create_async([=]()
    {
        bool containsEtag = false;
        Uri^ uri = FBClient::PrepareRequestUri(HttpMethod::Delete, path,
            parameters, nullptr, nullptr, containsEtag, nullptr);

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
    cancellation_token_source cancellationTokenSource =
        cancellation_token_source();

    return create_task(
        httpClient->DeleteAsync(RequestUri),
        cancellationTokenSource.get_token())
        .then([=](HttpResponseMessage^ response)
    {
        return create_task(response->Content->ReadAsStringAsync(),
            cancellationTokenSource.get_token());
    })
        .then([=](task<String^> previousTask)
    {
        String^ response = nullptr;

        try
        {
            // Check if any previous task threw an exception.
            response = previousTask.get();
        }
        catch (const task_canceled&)
        {
        }
        catch (Exception^ ex)
        {
        }

        return response;
    });
}

Uri^ FBClient::PrepareRequestUri(
    winsdkfb::HttpMethod httpMethod, 
    String^ path, 
    PropertySet^ parameters, 
    Type^ resultType, 
    Windows::Storage::Streams::IRandomAccessStream^ input,
    bool& containsEtag,
    Vector<int>^ batchEtags
    )
{
    batchEtags = nullptr;
    FBSession^ sess = FBSession::ActiveSession;

    // Setup datawriter for the InMemoryRandomAccessStream
    DataWriter^ dataWriter = ref new DataWriter(input);
    dataWriter->UnicodeEncoding = UnicodeEncoding::Utf8;
    dataWriter->ByteOrder = ByteOrder::LittleEndian;

    PropertySet^ mediaObjects = ref new PropertySet();
    PropertySet^ mediaStreams = ref new PropertySet();
    PropertySet^ parametersWithoutMediaObjects = ToDictionary(parameters, mediaObjects, mediaStreams);

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

    String^ contentTypeHeader = MultiPartContentType;
    String^ boundary = ref new String(MultiPartBoundary);
    contentTypeHeader += "boundary=" + boundary + MultiPartNewLine;
    long contentLength = -1;
    String^ queryString = ref new String();

    OutputDebugString(contentTypeHeader->Data());

    SerializeParameters(parametersWithoutMediaObjects);

    if (parametersWithoutMediaObjects->HasKey("access_token"))
    {
        // Add access_token to query string as the first parameter, for our own
        // convenience.  We could as easily add it in the while loop below, but
        // putting at the beginning of the query string is helpful for 
        // debugging, etc.
        auto accessToken = dynamic_cast<String^>(
            parametersWithoutMediaObjects->Lookup("access_token"));
        if ((accessToken != nullptr) && (accessToken->Length() > 0) && 
            (accessToken != "null"))
        {
            queryString += "access_token=" +  Uri::EscapeComponent(accessToken);
        }

        // Remove the token before we loop through and add general parameters
        parametersWithoutMediaObjects->Remove("access_token");
       
        // Add remaining parameters to query string.  Note that parameters that 
        // do not need to be uploaded as multipart, i.e. any which is are not 
        // binary data, are required to be in the query string, even for POST 
        // requests!
        auto kvp = parametersWithoutMediaObjects->First();
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

        if (mediaStreams->Size > 0)
        {
            IIterator<IKeyValuePair<String^, Object^>^>^ facebookMediaStream = 
                mediaStreams->First();
            while(facebookMediaStream->HasCurrent)
            {
                String^ sbMediaStream = ref new String();
                FBMediaStream^ mediaStream = 
                    dynamic_cast<FBMediaStream^>(
                        facebookMediaStream->Current->Value);

                if ((mediaStream->Stream == nullptr) ||
                    (mediaStream->Stream->ContentType == nullptr) || 
                    (mediaStream->FileName == nullptr) ||
                    (mediaStream->FileName->Length() == 0))
                {
                    throw ref new InvalidArgumentException(AttachmentMustHavePropertiesSetError);
                }

                sbMediaStream = MultiPartFormPrefix + boundary + 
                    MultiPartNewLine + 
                    "Content-Disposition: form-data; name=\"" +
                    facebookMediaStream->Current->Key + "\"; filename=\"" +
                    mediaStream->FileName + "\"" + MultiPartNewLine +
                    "Content-Type: " + mediaStream->Stream->ContentType +
                    MultiPartNewLine + MultiPartNewLine;
                OutputDebugString(sbMediaStream->Data());
                OutputDebugString(L"\n");

                dataWriter->WriteString(sbMediaStream);

                IRandomAccessStream^ stream = mediaStream->Stream;
                if (stream == nullptr)
                {
                    throw ref new InvalidArgumentException(AttachmentValueIsNull);
                }

                IInputStream^ inputStream = stream->GetInputStreamAt(0);
                DataReader^ dataReader = ref new DataReader(inputStream);
                dataReader->UnicodeEncoding = UnicodeEncoding::Utf8;
                dataReader->ByteOrder = ByteOrder::LittleEndian;

                dataWriter->WriteBuffer(dataReader->ReadBuffer(dataReader->UnconsumedBufferLength));

                dataWriter->WriteString(MultiPartNewLine);
                facebookMediaStream->MoveNext();
            }

            String^ str = ref new String();
            str = MultiPartNewLine + MultiPartFormPrefix + boundary +
                MultiPartFormPrefix + MultiPartNewLine;

            dataWriter->WriteString(str);
        }

        if (mediaObjects->Size > 0)
        {
            IIterator<IKeyValuePair<String^, Object^>^>^ facebookMediaObject =
                mediaObjects->First();
            while (facebookMediaObject->HasCurrent)
            {
                String^ sbMediaObject = ref new String();
                FBMediaObject^ mediaObject =
                    dynamic_cast<FBMediaObject^>(
                        facebookMediaObject->Current->Value);

                if ((mediaObject->GetValue()== nullptr) ||
                    (mediaObject->ContentType == nullptr) ||
                    (mediaObject->FileName == nullptr) ||
                    (mediaObject->FileName->Length() == 0))
                {
                    throw ref new InvalidArgumentException(AttachmentMustHavePropertiesSetError);
                }

                sbMediaObject = MultiPartFormPrefix + boundary +
                    MultiPartNewLine +
                    "Content-Disposition: form-data; name=\"" +
                    facebookMediaObject->Current->Key + "\"; filename=\"" +
                    mediaObject->FileName + "\"" + MultiPartNewLine +
                    "Content-Type: " + mediaObject->ContentType +
                    MultiPartNewLine + MultiPartNewLine;
                OutputDebugString(sbMediaObject->Data());
                OutputDebugString(L"\n");

                dataWriter->WriteString(sbMediaObject);

                if (mediaObject->GetValue() == nullptr)
                {
                    throw ref new InvalidArgumentException(AttachmentValueIsNull);
                }

                dataWriter->WriteBytes(mediaObject->GetValue());

                dataWriter->WriteString(MultiPartNewLine);
                facebookMediaObject->MoveNext();
            }

            String^ str = ref new String();
            str = MultiPartNewLine + MultiPartFormPrefix + boundary +
                MultiPartFormPrefix + MultiPartNewLine;

            dataWriter->WriteString(str);
        }

        // TODO: Figure out where to get the right value for this.  input 
        //doesn't appear to have a length at this point in the code when debugging,
        contentLength = input == nullptr ? 0 : (long)input->Size;
    }
    else
    {
        if (containsEtag && httpMethod != HttpMethod::Get)
        {
            String^ msg = ETagKey + L" is only supported for http get method.";
            throw ref new InvalidArgumentException(msg);
        }

        // for GET,DELETE
        if (mediaObjects->Size > 0 && mediaStreams->Size > 0)
        {
            throw ref new FailureException("Attachments (FBMediaObject/FBMediaStream) are valid only in POST requests.");
        }
    #if !WP8
        if (httpMethod == HttpMethod::Delete)
        {
            queryString += L"method=delete&";
        }
    #endif
        queryString += ParametersToQueryString(parametersWithoutMediaObjects);
    }

    String^ host;
    String^ apiVersion = L"";

    if (parametersWithoutMediaObjects->HasKey("request_host"))
    {
        host = static_cast<String^>(
            parametersWithoutMediaObjects->Lookup("request_host"));
    }
    else
    {
        host = L"graph.facebook.com";
        if (sess->APIMajorVersion)
        {
            apiVersion = L"v" + sess->APIMajorVersion.ToString() + L"." + sess->APIMinorVersion.ToString() + L"/";
        }
    }

    String^ uriString = L"https://" + host + L"/" + apiVersion + path + L"?" + queryString;

    return ref new Uri(uriString);
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
        // TODO: Jsonize the object value
        String^ newValue = dynamic_cast<String^>(parameters->Lookup(item->Current));

        // Replace the existing object with the new Jsonized value
        parameters->Remove(item->Current);
        parameters->Insert(item->Current, newValue);

        // Move to next
        item->MoveNext();
    }
}

String^ FBClient::ParametersToQueryString(
    PropertySet^ Parameters
    )
{
    String^ queryString = L"";

    // Add remaining parameters to query string.  Note that parameters that 
    // do not need to be uploaded as multipart, i.e. any which is are not 
    // binary data, are required to be in the query string, even for POST 
    // requests!
    IIterator<IKeyValuePair<String^, Object^>^>^ kvp = Parameters->First();
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

