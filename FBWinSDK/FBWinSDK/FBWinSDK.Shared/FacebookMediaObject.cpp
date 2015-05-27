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

#include "pch.h"
#include "FacebookMediaObject.h"

using namespace Platform;

namespace Facebook 
{
    String^ FBMediaObject::ContentType::get()
    {
        return this->_contentType;
    }
    void FBMediaObject::ContentType::set(String^ value)
    {
        this->_contentType = value;
    }

    String^ FBMediaObject::FileName::get()
    {
        return this->_fileName;
    }
    void FBMediaObject::FileName::set(String^ value)
    {
        this->_fileName = value;
    }

    FBMediaObject^ FBMediaObject::SetValue(
        const Array<unsigned char>^ value
        )
    {
        this->_value = ref new Array<unsigned char>(value);
        return this;
    }

    Array<unsigned char>^ FBMediaObject::GetValue(
        )
    {
        return this->_value;
    }
};
