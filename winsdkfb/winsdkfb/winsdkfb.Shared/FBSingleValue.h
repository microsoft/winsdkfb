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
#include "FacebookResult.h"
#include "HttpMethod.h"

namespace winsdkfb
{
    namespace Graph
    {
        /**
         * Used to interact with Facebook Graph API calls that return
         * non-paginated values.
         */
        public ref class FBSingleValue sealed
        {
        public:
            /**
             * Constructor.
             * @param Request The Graph API endpoint
             * @param Parameters Additional parameters that the Graph call may require
             * @param ObjectFactory Factory to create object from the response data
             */
            FBSingleValue(
                Platform::String^ Request,
                Windows::Foundation::Collections::PropertySet^ Parameters,
                FBJsonClassFactory^ ObjectFactory
                );

            /**
             * Makes an HTTP GET request to the Graph API endpoint.
             * @return On success, the returned FBResult will contain an object
             * from ObjectFactory that encapsulates the Graph call return data.
             * On failure, the FBResult will instead contain error information
             * about the call.
             */
            Windows::Foundation::IAsyncOperation<FBResult^>^ GetAsync(
                );

            /**
             * Makes an HTTP POST request to the Graph API endpoint.
             * @return On success, the returned FBResult will contain an object
             * from ObjectFactory that encapsulates the Graph call return data.
             * On failure, the FBResult will instead contain error information
             * about the call.
             */
            Windows::Foundation::IAsyncOperation<FBResult^>^ PostAsync(
                );

            /**
             * Makes an HTTP DELETE request to the Graph API endpoint.
             * @return On success, the returned FBResult will contain an object
             * from ObjectFactory that encapsulates the Graph call return data.
             * On failure, the FBResult will instead contain error information
             * about the call.
             */
            Windows::Foundation::IAsyncOperation<FBResult^>^ DeleteAsync(
                );

        private:
            FBResult^ ConsumeSingleValue(
                Platform::String^ JsonText
                );

            Windows::Foundation::IAsyncOperation<FBResult^>^ FBSingleValue::MakeHttpRequest(
                HttpMethod httpMethod
                );

            FBResult^ _result;
            Platform::String^ _request;
            Windows::Foundation::Collections::PropertySet^ _parameters;
            FBJsonClassFactory^ _objectFactory;
        };
    }
}
