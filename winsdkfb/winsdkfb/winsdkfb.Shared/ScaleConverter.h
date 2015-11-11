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
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class ScaleConverter sealed :
        Windows::UI::Xaml::Data::IValueConverter
    {
    public:
        virtual Platform::Object^ Convert(
            Platform::Object^ value,
            Windows::UI::Xaml::Interop::TypeName targetType,
            Platform::Object^ parameter,
            Platform::String^ language
            );

        virtual Platform::Object^ ConvertBack(
            Platform::Object^ value,
            Windows::UI::Xaml::Interop::TypeName targetType,
            Platform::Object^ parameter,
            Platform::String^ language
            )
        {
            throw ref new Platform::NotImplementedException();
        }
    };
}

