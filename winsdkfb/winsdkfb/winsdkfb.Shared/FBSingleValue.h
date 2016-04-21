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

namespace winsdkfb
{
    namespace Graph
    {
        /**
         * @brief Used to interact with Facebook Graph API calls that return
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
            /**
             * Attempts to use ObjectFactory to create an object from values
             * contained in JsonText.
             * @param JsonText Serialized json text that contains values used to
             * instantiate an object of the type created by ObjectFactory.
             * @return FBResult that contains a new object created by
             * ObjectFactory. On failure, the FBResult will instead contain an
             * FBError object.
             * @exception InvalidArgumentException if ObjectyFactory is unable
             * to instantiate an object or if the JsonText is unparsable.
             */
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
