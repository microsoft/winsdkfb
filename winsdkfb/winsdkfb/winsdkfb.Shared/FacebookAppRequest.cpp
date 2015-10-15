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

#include "pch.h"
#include "FacebookAppRequest.h"

using namespace winsdkfb;
using namespace Platform;
using namespace Platform::Collections;
using namespace std;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

Platform::String^ FBAppRequest::RequestId::get()
{
    return _RequestId;
}
        
RecipientView FBAppRequest::RecipientIds::get()
{
    Vector<String^>^ recips = ref new Vector<String^>(_Recipients);
    return recips->GetView();
}
        
FBAppRequest^ FBAppRequest::FromRequestDialogResponse(
    Uri^ Response
    )
{
    FBAppRequest^ info = nullptr;
    String^ requestId = nullptr;
    RecipientVector recips;

    if (Response->Query && !Response->Query->IsEmpty())
    {
        WwwFormUrlDecoder^ parameters = ref new WwwFormUrlDecoder(Response->Query);

        String^ postId = nullptr;

        for (unsigned int i = 0; i < parameters->Size; i++)
        {
            IWwwFormUrlDecoderEntry^ entry = parameters->GetAt(i);
            wstring name(entry->Name->Data());

            if (name == L"request")
            {
                requestId = entry->Value;
            }
            else if (name.find(L"to[") == 0)
            {
                recips.push_back(entry->Value);
            }
        }

        if (requestId && (recips.size() > 0))
        {
            info = ref new FBAppRequest(requestId, recips);
        }
    }

    return info;
}

FBAppRequest::FBAppRequest(
    String^ RequestId,
    const RecipientVector& Recipients
    ) :
    _RequestId(RequestId),
    _Recipients(Recipients)
{
    ;
}
