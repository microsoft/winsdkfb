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

namespace Facebook
{
    /*!\brief Container class holding the result of a graph API call - either a 
     * JSON object of some type, or an error, depending on the value of the
     * Succeeded property.
     */
    public ref class FBResult sealed
    {
        public:
            property bool Succeeded
            {
                bool get();
            }

            property Platform::Object^ Object
            {
                Platform::Object^ get();
            }

            property Facebook::FBError^ ErrorInfo
            {
                Facebook::FBError^ get();
            }

            FBResult(
                Platform::Object^ Object
                );
            
        private:
            ~FBResult(
                );

            Platform::Object^ _Object;
            Facebook::FBError^ _Error;
    };
}
