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
        /**
         * Used to interact with Facebook Graph API calls that return
         * paginated values.
         */
        public ref class FBPaginatedArray sealed
        {
        public:
            /**
             * Constructor.
             * @param Request The Graph API endpoint
             * @param Parameters Additional parameters that the Graph call may require
             * @param ObjectFactory Factory to create object from the response data
             */
            FBPaginatedArray(
                Platform::String^ Request,
                Windows::Foundation::Collections::PropertySet^ Parameters,
                winsdkfb::FBJsonClassFactory^ ObjectFactory
                );

            /**
             * Tries to get the first set of response objects from Facebook for the
             * Graph API call. Sets Current to the same IVectorView that is returned
             * by the FBResult on a successful call.
             * @exception InvalidArgumentException if ObjectFactory is unable to
             * instantiate the object it creates from the JSON values returned
             * from the Graph call.
             * @return FBResult object that encapsulates an IVectorView<Object^>^
             * where the Object^ can be cast to the object created by
             * ObjectFactory, if call was successful.
             */
            Windows::Foundation::IAsyncOperation<FBResult^>^ FirstAsync(
                );

            /**
             * Tries to get the next set of response objects from Facebook for the
             * Graph API call. Sets Current to the same IVectorView that is returned
             * by the FBResult on a successful call. Should only be called after
             * FirstAsync has been called.
             * @exception InvalidArgumentException if the current page does not
             * have a next page. This will also be be thrown if NextAsync is
             * called before FirstAsync. This will also be thrown if ObjectFactory
             * is unable to instantiate the object it creates from the JSON values
             * returned from the Graph call.
             * @return FBResult object that encapsulates an IVectorView<Object^>^
             * where the Object^ can be cast to the object created by
             * ObjectFactory, if call was successful.
             */
            Windows::Foundation::IAsyncOperation<FBResult^>^ NextAsync(
                );

            /**
             * Tries to get the previous set of response objects from Facebook for the
             * Graph API call. Sets Current to the same IVectorView that is returned
             * by the FBResult on a successful call. Should only be called after
             * FirstAsync has been called.
             * @exception InvalidArgumentException if the current page does not
             * have a previous page. This will also be be thrown if PreviousAsync is
             * called before FirstAsync. This will also be thrown if ObjectFactory
             * is unable to instantiate the object it creates from the JSON values
             * returned from the Graph call.
             * @return FBResult object that encapsulates an IVectorView<Object^>^
             * where the Object^ can be cast to the object created by
             * ObjectFactory, if call was successful.
             */
            Windows::Foundation::IAsyncOperation<FBResult^>^ PreviousAsync(
                );

            /**
             * The current collection of objects that were returned by a call
             * to FirstAsync, NextAsync, or PreviousAsync. The objects stored in
             * the IVectorView will have the type of the objects created by
             * ObjectFactory.
             */
            property Windows::Foundation::Collections::IVectorView<Object^>^
            Current
            {
                /**
                 * Gets the current collection of objects from the most recently
                 * queried page.
                 * @exception InvalidArgumentException if no page is the current page.
                 * @return An IVectorView^ of Object^ of the type created by
                 * ObjectFactory
                 */
                Windows::Foundation::Collections::IVectorView<Object^>^ get();
            }

            /**
             * Indicates if the Graph call successfully returned a page of data
             * that was successfully converted by ObjectFactory.
             */
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

            Windows::Foundation::IAsyncOperation<FBResult^>^ GetPage(
                Platform::String^ path
                );


            Windows::Foundation::Collections::IVectorView<Object^>^ _current;
            FBPaging^ _paging;
            Platform::String^ _request;
            Windows::Foundation::Collections::PropertySet^ _parameters;
            winsdkfb::FBJsonClassFactory^ _objectFactory;
        };
    }
}
