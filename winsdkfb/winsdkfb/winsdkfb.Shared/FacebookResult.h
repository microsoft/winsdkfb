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

#include "FacebookError.h"
#include "FacebookAccessTokenData.h"

namespace winsdkfb
{
    /*!\brief Container class holding the result of a graph API call - either a
     * JSON object of some type, or an error, depending on the value of the
     * Succeeded property. Note that the instantiation of Object and ErrorInfo
     * are exclusive to each other, only one can be instantiated at a time.
     */
    public ref class FBResult sealed
    {
        public:

            /**
             * Contructor for FBResult.
             * @param Object Additional information that FBResult can carry back
             * to the caller. If Object can be cast to FBError the FBResult will
             * be considered unsuccessful, otherwise it will be considered
             * successful.
             */
            FBResult(
                Platform::Object^ Object
                );

            /**
             * Indicates that the operation associated with the FBResult was
             * successful or not. Use to determine whether the Object or ErrorInfo
             * field should be accessed for more information.
             */
            property bool Succeeded
            {
                bool get();
            }

            /**
             * A context dependent object used to send additional data with the
             * FBResult. Object will need to be cast to different types depending
             * on the operation that generated it in order to be used.
             */
            property Platform::Object^ Object
            {
                Platform::Object^ get();
            }

            /**
             * Error information sent from the operation that created the
             * FBResult.
             */
            property winsdkfb::FBError^ ErrorInfo
            {
                winsdkfb::FBError^ get();
            }

        private:
            ~FBResult(
                );

            Platform::Object^ _Object;
            winsdkfb::FBError^ _Error;
    };
}
