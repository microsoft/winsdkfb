//******************************************************************************
//
// Copyright (c) 2016 Microsoft Corporation. All rights reserved.
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
#include "FBVideoContentStream.h"

using namespace concurrency;
using namespace Windows::Storage::Streams;
using namespace Windows::Foundation;

FBVideoContentStream::FBVideoContentStream(
        IRandomAccessStream^ stream,
        Platform::String^ contentType
    ) :
    _contentType{contentType},
    _stream{stream}
{
}

IAsyncOperationWithProgress<IBuffer^, unsigned int>^ FBVideoContentStream::ReadAsync(
    Windows::Storage::Streams::IBuffer^ buffer,
    unsigned int count,
    Windows::Storage::Streams::InputStreamOptions options
    )
{
    return _stream->ReadAsync(buffer, count, options);
}

IAsyncOperationWithProgress<unsigned int, unsigned int>^ FBVideoContentStream::WriteAsync(Windows::Storage::Streams::IBuffer^ buffer)
{
    return _stream->WriteAsync(buffer);
}

IAsyncOperation<bool>^ FBVideoContentStream::FlushAsync()
{
    return _stream->FlushAsync();
}

bool FBVideoContentStream::CanRead::get()
{
    return _stream->CanRead;
}

bool FBVideoContentStream::CanWrite::get()
{
    return _stream->CanWrite;
}

unsigned long long FBVideoContentStream::Position::get()
{
    return _stream->Position;
}

unsigned long long FBVideoContentStream::Size::get()
{
    return _stream->Size;
}

void FBVideoContentStream::Size::set(unsigned long long value)
{
    _stream->Size = value;
}

IInputStream^ FBVideoContentStream::GetInputStreamAt(unsigned long long position)
{
    return _stream->GetInputStreamAt(position);
}

IOutputStream^ FBVideoContentStream::GetOutputStreamAt(unsigned long long position)
{
    return _stream->GetOutputStreamAt(position);
}

void FBVideoContentStream::Seek(unsigned long long position)
{
    _stream->Seek(position);
}

IRandomAccessStream^ FBVideoContentStream::CloneStream()
{
    return ref new FBVideoContentStream(_stream->CloneStream(), _contentType);
}

Platform::String^ FBVideoContentStream::ContentType::get()
{
    return _contentType;
}

FBVideoContentStream::~FBVideoContentStream()
{
}

IAsyncOperation<FBVideoContentStream^>^ FBVideoContentStream::TruncateCloneStreamAsync(unsigned long long position, unsigned int size)
{
    unsigned long long oldPosition = _stream->Position;
    _stream->Seek(position);
    IBuffer^ buffer = ref new Buffer(size);

    task<FBVideoContentStream^> workTask = create_task(_stream->ReadAsync(buffer, size, InputStreamOptions::None)).then([=](IBuffer^ filledBuffer)
    {
        _stream->Seek(oldPosition);
        InMemoryRandomAccessStream^ memoryStream = ref new InMemoryRandomAccessStream();
        return create_task(memoryStream->WriteAsync(filledBuffer)).then([=](unsigned int count)
        {
            memoryStream->Seek(0);
            return ref new FBVideoContentStream(memoryStream, _contentType);
        });
    });
    return create_async([=]()
    {
        return workTask;
    });
}
