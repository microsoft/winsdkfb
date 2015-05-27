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
    public ref class FBMediaObject sealed
    {
        public:
            property Platform::String^ ContentType
            { 
                Platform::String^ get (); 
                void set(Platform::String^); 
            }

            property Platform::String^ FileName 
            { 
                Platform::String^ get(); 
                void set(Platform::String^); 
            }

            FBMediaObject^ SetValue(
                const Platform::Array<unsigned char>^ value
                );

            Platform::Array<unsigned char>^ GetValue(
                );
        
        private:
            Platform::Array<unsigned char>^ _value;
            Platform::String^ _contentType;
            Platform::String^ _fileName;
    };

};
