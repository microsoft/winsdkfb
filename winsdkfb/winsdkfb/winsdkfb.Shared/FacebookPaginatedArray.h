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

#include "JsonClassFactory.h"
#include "FBPaging.h"
#include "FacebookClient.h"
#include "FacebookResult.h"

namespace winsdkfb
{
    namespace Graph
    {
        public ref class FBPaginatedArray sealed
        {
        public:
            FBPaginatedArray(
                Platform::String^ Request,
                Windows::Foundation::Collections::PropertySet^ Parameters,
                winsdkfb::FBJsonClassFactory^ ObjectFactory
                );

            Windows::Foundation::IAsyncOperation<FBResult^>^ FirstAsync(
                );

            Windows::Foundation::IAsyncOperation<FBResult^>^ NextAsync(
                );

            Windows::Foundation::IAsyncOperation<FBResult^>^ PreviousAsync(
                );
            
            property Windows::Foundation::Collections::IVectorView<Object^>^ 
            Current
            {
                Windows::Foundation::Collections::IVectorView<Object^>^ get();
            }

            property bool HasCurrent
            {
                bool get();
            }
            
            property bool HasNext
            {
                bool get();
            }

            property bool HasPrevious
            {
                bool get();
            }

            Windows::Foundation::Collections::IVectorView<Object^>^
            ObjectArrayFromWebResponse(
                Platform::String^ Response,
                FBJsonClassFactory^ classFactory
                );
            
        private:
            Windows::Foundation::Collections::IVectorView<Object^>^ 
            ObjectArrayFromJsonArray(
                Windows::Data::Json::JsonArray^ Values,
                FBJsonClassFactory^ ClassFactory
                );

            FBResult^ ConsumePagedResponse(
                Platform::String^ JsonText
                );

            Windows::Foundation::Collections::IVectorView<Object^>^ _current;
            FBPaging^ _paging;
            Platform::String^ _request;
            Windows::Foundation::Collections::PropertySet^ _parameters;
            winsdkfb::FBJsonClassFactory^ _objectFactory;
        };
    }
}
