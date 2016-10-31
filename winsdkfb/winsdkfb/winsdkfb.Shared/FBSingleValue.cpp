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
#include "SDKMessage.h"
#include "FacebookResult.h"
#include "FacebookSession.h"
#include "FBSingleValue.h"
#include "HttpManager.h"

using namespace concurrency;
using namespace winsdkfb;
using namespace winsdkfb::Graph;
using namespace Platform;
using namespace Windows::Data::Json;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

FBSingleValue::FBSingleValue(
    Platform::String^ Request,
    ::PropertySet^ Parameters,
    winsdkfb::FBJsonClassFactory^ ObjectFactory
    ) :
    _request(Request),
    _parameters(Parameters),
    _objectFactory(ObjectFactory)
{
}

Windows::Foundation::IAsyncOperation<FBResult^>^ FBSingleValue::GetAsync(
    )
{
    return MakeHttpRequest(HttpMethod::Get);
}

Windows::Foundation::IAsyncOperation<FBResult^>^ FBSingleValue::PostAsync(
    )
{
    return MakeHttpRequest(HttpMethod::Post);
}

Windows::Foundation::IAsyncOperation<FBResult^>^ FBSingleValue::DeleteAsync(
    )
{
    return MakeHttpRequest(HttpMethod::Delete);
}


FBResult^ FBSingleValue::ConsumeSingleValue(
    String^ JsonText
    )
{
    FBResult^ result = nullptr;
    JsonValue^ Value = nullptr;
    Object^ item = nullptr;
    FBError^ err = nullptr;

    if (JsonValue::TryParse(JsonText, &Value))
    {
        if (Value->ValueType == JsonValueType::Object)
        {
            //TODO: Check for error here first.  User's object serializer may
            //produce a false positive.
            err = FBError::FromJson(JsonText);
            if (!err)
            {
                item = _objectFactory(JsonText);
                if (!item)
                {
                    JsonObject^ obj = Value->GetObject();
                    IIterator<IKeyValuePair<String^, IJsonValue^>^>^ it = 
                        nullptr;
                    for (it = obj->First(); it->HasCurrent; it->MoveNext())
                    {
                        if (!String::CompareOrdinal(it->Current->Key, 
                            L"error"))
                        {
                            err = FBError::FromJson(
                                it->Current->Value->Stringify());
                            break;
                        }
                        else if (!String::CompareOrdinal(it->Current->Key, 
                            L"data"))
                        {
                            if (it->Current->Value->ValueType != 
                                JsonValueType::Object)
                            {
                                throw ref new InvalidArgumentException(
                                    SDKMessageBadObject);
                            }
                            item = 
                                _objectFactory(it->Current->Value->Stringify());
                            if (!item)
                            {
                                throw ref new InvalidArgumentException(
                                    SDKMessageBadObject);
                            }
                        }
                    }
                }
            }
        }

        if (item)
        {
            result = ref new FBResult(item);
        }
        else if (err)
        {
            result = ref new FBResult(err);
        }
        else
        {
            //Didn't find anything...
            throw ref new InvalidArgumentException(SDKMessageNoData);
        }
    }

    return result;
}

Windows::Foundation::IAsyncOperation<FBResult^>^ FBSingleValue::MakeHttpRequest(
    HttpMethod httpMethod
)
{
    if (_parameters == nullptr)
    {
        _parameters = ref new PropertySet();
    }
    return create_async([=]()->task<FBResult^>
    {
        task<String^> innerTask;
        switch (httpMethod)
        {
        case HttpMethod::Get:
            innerTask = create_task(HttpManager::Instance->GetTaskAsync(_request, _parameters->GetView()));
            break;
        case HttpMethod::Post:
            innerTask = create_task(HttpManager::Instance->PostTaskAsync(_request, _parameters->GetView()));
            break;
        case HttpMethod::Delete:
            innerTask = create_task(HttpManager::Instance->DeleteTaskAsync(_request, _parameters->GetView()));
            break;
        default:
            OutputDebugString(L"FBSingleValue::MakeHttpRequest recieved unknown HttpMethod value\n");
            innerTask = create_task([]() -> String^ { return nullptr; });
            break;
        }
        return innerTask.then([this](String^ responseString) -> FBResult^
        {
            if (responseString == nullptr)
            {
                return ref new FBResult(ref new FBError(0, L"HTTP request failed", "unable to receive response"));
            }
            else
            {
                return ConsumeSingleValue(responseString);
            }
        });
    });
}
