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

#pragma once

using namespace Windows::Storage::Streams;

ref class FBVideoContentStream sealed : IRandomAccessStreamWithContentType
{
public:

    FBVideoContentStream(
        IRandomAccessStream^ stream,
        Platform::String^ contentType
    );

    // Inherited via IRandomAccessStream
    virtual Windows::Foundation::IAsyncOperationWithProgress<Windows::Storage::Streams::IBuffer^, unsigned int>^
    ReadAsync(
        Windows::Storage::Streams::IBuffer^buffer,
        unsigned int count,
        Windows::Storage::Streams::InputStreamOptions options
        );

    virtual Windows::Foundation::IAsyncOperationWithProgress<unsigned int, unsigned int>^
    WriteAsync(
        Windows::Storage::Streams::IBuffer^buffer
        );

    virtual Windows::Foundation::IAsyncOperation<bool>^ FlushAsync();

    virtual property bool CanRead
    {
        bool get();
    }

    virtual property bool CanWrite
    {
        bool get();
    }

    virtual property unsigned long long Position
    {
        unsigned long long get();
    }

    virtual property unsigned long long Size
    {
        unsigned long long get();
        void set(unsigned long long);
    }

    virtual Windows::Storage::Streams::IInputStream^ GetInputStreamAt(
        unsigned long long position
        );

    virtual Windows::Storage::Streams::IOutputStream^ GetOutputStreamAt(
        unsigned long long position
        );

    virtual void Seek(
        unsigned long long position
        );

    virtual Windows::Storage::Streams::IRandomAccessStream^ CloneStream();

    // Inherited via IContentTypeProvider
    virtual property Platform::String^ ContentType
    {
        Platform::String^ get();
    }

    virtual ~FBVideoContentStream();

    /**
     * Clones a portion of the stream into another object.
     * @param position The position in the stream to start the clone at
     * @param size The amount of bytes to clone
     * @return The cloned, truncated FBVideoContentStream
     */
    Windows::Foundation::IAsyncOperation<FBVideoContentStream^>^ TruncateCloneStreamAsync(
        unsigned long long position,
        unsigned int size
        );
private:
    Platform::String^ _contentType;
    IRandomAccessStream^ _stream;
};
