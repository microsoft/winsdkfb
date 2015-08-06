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
#include "FacebookProfilePicture.h"

using namespace Facebook;
using namespace Platform;
using namespace Windows::Data::Json;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

Uri^ FBProfilePicture::URL::get()
{
    return _Uri;
}

void FBProfilePicture::URL::set(Uri^ value)
{
    _Uri = value;
}

bool FBProfilePicture::IsSilhouette::get()
{
    return _IsSilhouette;
}

void FBProfilePicture::IsSilhouette::set(bool value)
{
    _IsSilhouette = value;
}

int FBProfilePicture::Height::get()
{
    return _Height;
}

void FBProfilePicture::Height::set(int value)
{
    _Height = value;
}

int FBProfilePicture::Width::get()
{
    return _Width;
}

void FBProfilePicture::Width::set(int value)
{
    _Width = value;
}

 FBProfilePicture^ FBProfilePicture::FromJson(
    String^ JsonText
    )
{
    FBProfilePicture^ result = nullptr;
    JsonValue^ val = nullptr;
    if (JsonValue::TryParse(JsonText, &val))
    {
        if (val->ValueType == JsonValueType::Object)
        {
            JsonObject^ obj = val->GetObject();
            IIterator<IKeyValuePair<String^, IJsonValue^>^>^ it = nullptr;
            for (it = obj->First(); it->HasCurrent; it->MoveNext())
            {
                if (!String::CompareOrdinal(it->Current->Key, L"data"))
                {
                    if (it->Current->Value->ValueType == JsonValueType::Object)
                    {
                        JsonObject^ innerObject = it->Current->Value->GetObject();
                        IIterator<IKeyValuePair<String^, IJsonValue^>^>^ it2 = nullptr;
                        result = ref new FBProfilePicture();
                        for (it2 = innerObject->First(); it2->HasCurrent; it2->MoveNext())
                        {
                            String^ key = it2->Current->Key;
                            if (!String::CompareOrdinal(key, L"url"))
                            {
                                result->_Uri = ref new Uri(it2->Current->Value->GetString());
                            }
                            if (!String::CompareOrdinal(key, L"is_silhouette"))
                            {
                                result->_IsSilhouette = it2->Current->Value->GetBoolean();
                            }
                            if (!String::CompareOrdinal(key, L"height"))
                            {
                                result->_Height = static_cast<int>(it2->Current->Value->GetNumber());
                            }
                            if (!String::CompareOrdinal(key, L"width"))
                            {
                                result->_Width = static_cast<int>(it2->Current->Value->GetNumber());
                            }
                        }
                    }
                }
            }
        }
    }

    return result;
}

FBProfilePicture::FBProfilePicture() :
    _Uri(nullptr),
    _IsSilhouette(true),
    _Height(0),
    _Width(0)
{
}
