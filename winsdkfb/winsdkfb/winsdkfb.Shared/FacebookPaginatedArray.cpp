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
#include "FacebookPaginatedArray.h"
#include "FacebookResult.h"
#include "FacebookSession.h"
#include "HttpManager.h"

using namespace concurrency;
using namespace winsdkfb;
using namespace winsdkfb::Graph;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Data::Json;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Web::Http;
using namespace Windows::Web::Http::Filters;

FBPaginatedArray::FBPaginatedArray(
    Platform::String^ Request,
    PropertySet^ Parameters,
    winsdkfb::FBJsonClassFactory^ ObjectFactory
    ) :
    _current(nullptr),
    _request(Request),
    _parameters(Parameters),
    _objectFactory(ObjectFactory)
{
}

Windows::Foundation::IAsyncOperation<FBResult^>^ FBPaginatedArray::FirstAsync(
    )
{
    return GetPage(_request);
}

Windows::Foundation::IAsyncOperation<FBResult^>^ FBPaginatedArray::NextAsync(
    )
{
    if (!HasNext)
    {
        return create_async([this]()
        {
            return ref new FBResult(ref new FBError(0, L"Invalid SDK call", L"No next page"));
        });
    }
    return GetPage(_paging->Next);
}

Windows::Foundation::IAsyncOperation<FBResult^>^ FBPaginatedArray::PreviousAsync(
    )
{
    if (!HasPrevious)
    {
        return create_async([this]()
        {
            return ref new FBResult(ref new FBError(0, L"Invalid SDK call", L"No previous page"));
        });
    }
    return GetPage(_paging->Previous);
}

IVectorView<Object^>^ FBPaginatedArray::Current::get()
{
    IVectorView<Object^>^ result = nullptr;

    if (!HasCurrent)
    {
        throw ref new InvalidArgumentException(SDKMessageBadCall);
    }

    return _current;
}

bool FBPaginatedArray::HasCurrent::get()
{
    return (_current != nullptr);
}

bool FBPaginatedArray::HasNext::get()
{
    return (_paging && (_paging->Next != nullptr));
}

bool FBPaginatedArray::HasPrevious::get()
{
    return (_paging && (_paging->Previous != nullptr));
}

IVectorView<Object^>^ FBPaginatedArray::ObjectArrayFromJsonArray(
    JsonArray^ Values,
    winsdkfb::FBJsonClassFactory^ ClassFactory
    )
{
    Vector<Object^>^ result = ref new Vector<Object^>(0);
    unsigned int index = 0;
    for (IIterator<IJsonValue^>^ it = Values->First();
        it->HasCurrent;
        it->MoveNext())
    {
        String^ jsonText = it->Current->Stringify();
        Object^ item = ClassFactory(jsonText);
        if (!item)
        {
            throw ref new InvalidArgumentException(SDKMessageBadObject);
        }
        result->Append(item);
        index++;
    }

    return result->GetView();
}

FBResult^ FBPaginatedArray::ConsumePagedResponse(
    String^ JsonText
    )
{
    FBResult^ result = nullptr;
    JsonValue^ Value = nullptr;
    bool foundPaging = false;
    bool foundData = false;
    bool foundError = false;

    if (JsonValue::TryParse(JsonText, &Value))
    {
        if (Value->ValueType == JsonValueType::Object)
        {
            JsonObject^ obj = Value->GetObject();
            IIterator<IKeyValuePair<String^, IJsonValue^>^>^ it = nullptr;
            for (it = obj->First(); it->HasCurrent; it->MoveNext())
            {
                if (!String::CompareOrdinal(it->Current->Key, L"error"))
                {
                    FBError^ err = FBError::FromJson(
                        it->Current->Value->Stringify());
                    result = ref new FBResult(err);
                    foundError = true;
                    break;
                }
                else if (!String::CompareOrdinal(it->Current->Key, L"paging"))
                {
                    _paging = static_cast<FBPaging^>(
                        FBPaging::FromJson(it->Current->Value->Stringify()));
                    if (_paging)
                    {
                        foundPaging = true;
                    }
                }
                else if (!String::CompareOrdinal(it->Current->Key, L"data"))
                {
                    if (it->Current->Value->ValueType != JsonValueType::Array)
                    {
                        throw ref new InvalidArgumentException(
                            SDKMessageBadObject);
                    }

                    _current = ObjectArrayFromJsonArray(
                        it->Current->Value->GetArray(), _objectFactory);
                    if (_current)
                    {
                        foundData = true;
                    }
                }
            }
        }

        // If all data for the array fits in one page, we don't get a 'paging'
        // object.  We should always get a data object, though. Note that a FB
        // error should not result in a thrown exception!
        if (!(foundError || foundData))
        {
            throw ref new InvalidArgumentException(SDKMessageBadObject);
        }

        if (!foundError)
        {
            result = ref new FBResult(_current);
        }
    }

    return result;
}

IVectorView<Object^>^ FBPaginatedArray::ObjectArrayFromWebResponse(
    String^ Response,
    FBJsonClassFactory^ classFactory
    )
{
    IVectorView<Object^>^ result = nullptr;
    JsonObject^ rootObject = nullptr;
    if (JsonObject::TryParse(Response, &rootObject))
    {
        IIterator<IKeyValuePair<String^, IJsonValue^>^>^ it = nullptr;
        for (it = rootObject->First(); it->HasCurrent; it->MoveNext())
        {
            String^ key = it->Current->Key;
            JsonValueType t = it->Current->Value->ValueType;

            if (!(String::CompareOrdinal(key, "data")) &&
                (t == JsonValueType::Array))
            {
                result = ObjectArrayFromJsonArray(
                    it->Current->Value->GetArray(), classFactory);
            }
        }
    }

    return result;
}

Windows::Foundation::IAsyncOperation<FBResult^>^ FBPaginatedArray::GetPage(
    String^ path
    )
{
    return create_async([this, path]() -> task<FBResult^>
    {
        return create_task(HttpManager::Instance->GetTaskAsync(path, _parameters))
            .then([this](String^ responseString)
        {
            if (responseString == nullptr)
            {
                return ref new FBResult(ref new FBError(0, L"HTTP request failed", "unable to receive response"));
            }
            else
            {
                return ConsumePagedResponse(responseString);
            }
        });
    });
}
