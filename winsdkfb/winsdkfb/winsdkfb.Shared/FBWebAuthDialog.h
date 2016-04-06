//******************************************************************************
//
// Copyright (c) 2016 Microsoft Corporation. All rights reserved.
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
#include "FacebookResult.h"
#include "FacebookError.h"

namespace winsdkfb
{

	[Windows::Foundation::Metadata::WebHostHidden]
    public ref class FBWebAuthDialog sealed
    {
    public:
        static Windows::Foundation::IAsyncOperation<winsdkfb::FBResult^>^ ShowFeedDialog(
            Windows::Foundation::Collections::PropertySet^ Parameters
            );

        static Windows::Foundation::IAsyncOperation<winsdkfb::FBResult^>^ ShowRequestsDialog(
            Windows::Foundation::Collections::PropertySet^ Parameters
            );

        static Windows::Foundation::IAsyncOperation<winsdkfb::FBResult^>^ ShowSendDialog(
            Windows::Foundation::Collections::PropertySet^ Parameters
            );

    private:
        static Windows::Foundation::IAsyncOperation<winsdkfb::FBResult^>^ ShowDialog(
            Windows::Foundation::Uri^ DialogUri,
            std::function<FBResult^(Platform::String^)> SuccessResultFunc
            );

        static FBError^ UserCanceledError();
        static FBError^ MalformedResponseError(Platform::String^ ResponseText);
    };
}
