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
#include "FacebookClient.h"
#include "FacebookResult.h"

namespace Facebook
{
    namespace Graph
    {
        public ref class FBSingleValue sealed
        {
        public:
            FBSingleValue(
                Platform::String^ Request,
                Windows::Foundation::Collections::PropertySet^ Parameters,
                FBJsonClassFactory^ ObjectFactory
                );

            Windows::Foundation::IAsyncOperation<FBResult^>^ Get(
                );

            Windows::Foundation::IAsyncOperation<FBResult^>^ Post(
                );

            Windows::Foundation::IAsyncOperation<FBResult^>^ Delete(
                );

        private:
            FBResult^ ConsumeSingleValue(
                Platform::String^ JsonText
                );

            FBResult^ _result;
            Platform::String^ _request;
            Windows::Foundation::Collections::PropertySet^ _parameters;
            FBJsonClassFactory^ _objectFactory;
        };
    }
}
