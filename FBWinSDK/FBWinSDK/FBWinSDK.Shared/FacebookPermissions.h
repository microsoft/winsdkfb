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
    /*!\brief Simple class to contain a set of permissions for FB.  
    */
    public ref class FBPermissions sealed
	{
		public:
            //! Construct an instance with a given set of permissions
            FBPermissions(
                Windows::Foundation::Collections::IVectorView<Platform::String^>^ Permissions
                );

            //! The list of permissions
			property Windows::Foundation::Collections::IVectorView<Platform::String^>^ Values
			{
				Windows::Foundation::Collections::IVectorView<Platform::String^>^ get();
			};

            //! Handy override that provides a string version of permissions in
            // the format suitable for concatenating into a FB request URL.
			virtual Platform::String^ ToString(
				) override;

		private:
			Windows::Foundation::Collections::IVectorView<Platform::String^>^ _values;
	};
}

