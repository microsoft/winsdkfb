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
#include "FacebookMediaStream.h"

using namespace Platform;
using namespace Windows::Storage::Streams;

namespace Facebook 
{
    FBMediaStream::FBMediaStream(
        String^ FileName,
        IRandomAccessStreamWithContentType^ Stream
        )
    {
        _fileName = FileName;
        _stream = Stream;
    }

    IRandomAccessStreamWithContentType^ FBMediaStream::Stream::get()
    {
        return _stream;
    }

    String^ FBMediaStream::FileName::get()
    {
        return _fileName;
    }
};
