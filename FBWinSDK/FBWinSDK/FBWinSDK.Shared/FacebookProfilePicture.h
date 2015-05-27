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
    /*!\brief Facebook user info class.
     */
    public ref class FBProfilePicture sealed
    {
    public:
        static FBProfilePicture^ FromJson(
            Platform::String^ JsonText
            );

        property Windows::Foundation::Uri^ URL
        {
            Windows::Foundation::Uri^ get();
            void set(Windows::Foundation::Uri^ value);
        }

        property bool IsSilhouette
        {
            bool get();
            void set(bool value);
        }

        property int Height
        {
            int get();
            void set(int value);
        }
        
        property int Width 
        {
            int get();
            void set(int value);
        }

    private:
        FBProfilePicture(
            );

        Windows::Foundation::Uri^ m_Uri;
        bool m_IsSilhouette;
        int  m_Height;
        int  m_Width;
    };
}
