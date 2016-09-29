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

namespace winsdkfb
{
    /**
     * @brief Class that encapsulates the response from a successful use of the
     * Facebook "Feed" dialog.
     */
    public ref class FBFeedRequest sealed
    {
    public:

        /**
         * ID of the post. Can be used to refer to this post in the future,
         * e.g. to delete or update it.
         */
        property Platform::String^ PostId
        {
            Platform::String^ get();
        }
        
        /**
         * Contruct a FBFeedRequest object from a URI.
         * @param Response The URI to create a FBFeedRequest object from
         * @return FBFeedRequest representation of the URI if it is valid,
         * otherwise nullptr
         */
        static FBFeedRequest^ FromFeedDialogResponse(
            Windows::Foundation::Uri^ Response
            );

    private:

        Platform::String^ _PostId;

        FBFeedRequest(
            Platform::String^ PostId
            );
    };
}
