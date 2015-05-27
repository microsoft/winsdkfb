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

namespace Facebook
{
    /*!\brief Class that encapsulates the response from a successful use of the
     * Facebook "App Requests" dialog.
     */
    public ref class FBFeedRequest sealed
    {
    public:

        //! Post ID - app can refer to this post in future, e.g. to delete or
        //update it.
        property Platform::String^ PostId
        {
            Platform::String^ get();
        }
        
        static FBFeedRequest^ FromFeedDialogResponse(
            Windows::Foundation::Uri^ Response
            );

    private:

        Platform::String^ m_PostId;

        FBFeedRequest(
            Platform::String^ PostId
            );
    };
}
