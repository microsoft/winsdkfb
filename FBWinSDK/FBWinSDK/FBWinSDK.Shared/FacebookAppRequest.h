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

//#include <collection.h>

namespace Facebook
{
    typedef std::vector<Platform::String^> RecipientVector;

    typedef Windows::Foundation::Collections::IVectorView<Platform::String^>^
        RecipientView;

    /*!\brief Class that encapsulates the response from a successful use of the
     * Facebook "App Requests" dialog.
     */
    public ref class FBAppRequest sealed
    {
    public:

        //! Request ID - app is responsible for tracking these! Once a request
        //! has been accepted, FB notifies the app's web site(?verify this), and
        //! the app (or web site, or whatever) must make a graph API call to FB
        //! to delete it.
        property Platform::String^ RequestId
        {
            Platform::String^ get();
        }
        
        //! Recipient ID(s) - the list of user IDs of people this request went
        //! to.
        property RecipientView RecipientIds
        {
            RecipientView get();
        }

        static FBAppRequest^ FromRequestDialogResponse(
            Windows::Foundation::Uri^ Response
            );

    private:

        RecipientVector _Recipients;
        Platform::String^ _RequestId;

        FBAppRequest(
            Platform::String^ RequestId,
            const RecipientVector& Recipients
            );
    };
}
