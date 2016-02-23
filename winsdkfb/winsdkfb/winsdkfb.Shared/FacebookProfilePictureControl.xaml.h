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

//
// ProfilePictureControl.xaml.h
// Declaration of the ProfilePictureControl class
//

#pragma once

#include "FacebookProfilePictureControl.g.h"
#include "FBProfilePicture.h"
#include "FacebookResult.h"

using namespace Platform;
using namespace Windows::UI::Xaml::Controls;

namespace winsdkfb
{
    public enum class CroppingType 
    {
        Square = 0,
        Original = 1
    };

	[Windows::Foundation::Metadata::WebHostHidden]
    public ref class ProfilePictureControl sealed
	{
	public:
		ProfilePictureControl();

        property String^ UserId
        {
            String^ get();
            void set(String^ value);
        }

        property CroppingType CropMode
        {
            CroppingType get();
            void set(CroppingType value);
        }

    private:
        void SetImageSourceFromUserId(
            );

        void SetImageSourceFromResource(
            );

        void Update(
            );

        concurrency::task<winsdkfb::FBResult^>
        GetProfilePictureInfo(
            int width,
            int height
            );

        bool _userIdValid;
        String^ _UserId;
        CroppingType _CropMode;
	};
}
