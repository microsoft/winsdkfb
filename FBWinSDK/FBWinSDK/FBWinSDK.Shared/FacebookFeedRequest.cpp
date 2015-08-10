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
#include "FacebookFeedRequest.h"

using namespace Facebook;
using namespace Platform;
using namespace std;
using namespace Windows::Foundation;

String^ FBFeedRequest::PostId::get()
{
    return _PostId;
}
        
FBFeedRequest^ FBFeedRequest::FromFeedDialogResponse(
    Uri^ Response
    )
{
    FBFeedRequest^ info = nullptr;
    WwwFormUrlDecoder^ parameters = nullptr;
    
    if (Response->Query)
    {
        parameters = ref new WwwFormUrlDecoder(Response->Query);
    }

    String^ postId = nullptr;

    for (unsigned int i = 0; parameters && i < parameters->Size; i++)
    {
        IWwwFormUrlDecoderEntry^ entry = parameters->GetAt(i);

        if (entry->Name->Equals(L"post_id"))
        {
            postId = entry->Value;
        }
    }

    if (postId)
    {
        info = ref new FBFeedRequest(postId);
    }
    return info;
}

FBFeedRequest::FBFeedRequest(
    String^ PostId
    ) :
    _PostId(PostId)
{
    ;
}
