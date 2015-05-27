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
    return m_Uri;
}

void FBProfilePicture::URL::set(Uri^ value)
{
    m_Uri = value;
}

bool FBProfilePicture::IsSilhouette::get()
{
    return m_IsSilhouette;
}

void FBProfilePicture::IsSilhouette::set(bool value)
{
    m_IsSilhouette = value;
}

int FBProfilePicture::Height::get()
{
    return m_Height;
}

void FBProfilePicture::Height::set(int value)
{
    m_Height = value;
}

int FBProfilePicture::Width::get()
{
    return m_Width;
}

void FBProfilePicture::Width::set(int value)
{
    m_Width = value;
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
                                result->m_Uri = ref new Uri(it2->Current->Value->GetString());
                            }
                            if (!String::CompareOrdinal(key, L"is_silhouette"))
                            {
                                result->m_IsSilhouette = it2->Current->Value->GetBoolean();
                            }
                            if (!String::CompareOrdinal(key, L"height"))
                            {
                                result->m_Height = static_cast<int>(it2->Current->Value->GetNumber());
                            }
                            if (!String::CompareOrdinal(key, L"width"))
                            {
                                result->m_Width = static_cast<int>(it2->Current->Value->GetNumber());
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
    m_Uri(nullptr),
    m_IsSilhouette(true),
    m_Height(0),
    m_Width(0)
{
}
