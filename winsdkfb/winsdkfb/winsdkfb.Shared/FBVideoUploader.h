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

#include "FacebookResult.h"
#include "FBVideoContentStream.h"
#include "FBVideo.h"

namespace winsdkfb
{
    //! Specifies which graph api method to use to upload the video.
    public enum class VideoUploadBehavior
    {
        SingleRequest, // one http request is made
        MultiRequest // video is uploaded in chunks
    };

    /**
     * @brief Provides video uploading from a local file to Facebook.
     */
    public ref class FBVideoUploader sealed
    {
    public:
        /**
         * Uploads a video from a local file to Facebook.
         * @param videoFile The video file to upload
         * @return FBResult indicating the result of the operation. On success,
         * it will contain an FBVideo object and on failure it will contain an
         * FBError object.
         */
        static Windows::Foundation::IAsyncOperation<FBResult^>^ UploadVideoAsync(
            Windows::Storage::StorageFile^ videoFile
            );

        /**
         * Uploads a video from a local file to Facebook.
         * @param videoFile The video file to upload
         * @param uploadBehavior The way the video should be uploaded
         * @return FBResult indicating the result of the operation. On success,
         * it will contain an FBVideo object and on failure it will contain an
         * FBError object.
         */
        static Windows::Foundation::IAsyncOperation<FBResult^>^ UploadVideoAsync(
            Windows::Storage::StorageFile^ videoFile,
            VideoUploadBehavior uploadBehavior
            );

    private:
        /**
         * Uploads a video in one HTTP request. For videos that are small enough
         * to meet Facebook's guidelines on small video uploading.
         * @param videoFile The video file to upload
         * @return FBResult indicating the result of the operation, will contain
         * an FBVideo object on success and and FBError object on error.
         */
        static Concurrency::task<FBResult^> UploadSmallVideo(
            Windows::Storage::StorageFile^ videoFile
            );

        /**
         * Uploads a video with multiple HTTP requests. For videos that are too
         * big to be uploading with UploadSmallVideo().
         * @param videoFile The video file to upload
         * @param fileSize The size of the video file, in bytes
         * @return FBResult indicating the result of the operation, will contain
         * an FBVideo object on success and and FBError object on error.
         */
        static Concurrency::task<FBResult^> UploadLargeVideo(
            Windows::Storage::StorageFile^ videoFile,
            unsigned long long fileSize
            );

        /**
         * Starts the large video uploading process.
         * @param fileSize The size of the video file to upload, in bytes
         * @return FBResult indicating the result of the operation, will contain
         * an FBVideo object on success and and FBError object on error.
         */
        static Concurrency::task<FBResult^> StartRequest(
            unsigned long long fileSize
            );

        /**
         * Portions the video file into chunks and uploads them to sequentially.
         * @param stream The stream of the video file data
         * @param video The FBVideo object containing the upload process data
         * from previous operations
         * @param title The name of the video file to upload
         * @return FBResult indicating the result of the operation, will contain
         * an FBVideo object on success and and FBError object on error.
         */
        static Concurrency::task<FBResult^> TransferRequest(
            FBVideoContentStream^ stream,
            Graph::FBVideo^ video,
            Platform::String^ title
            );

        /**
         * Signals to Facebook the end of the large video upload process
         * @param uploadSessionId The session ID of the video upload
         * @return FBResult indicating the result of the operation, will contain
         * an FBVideo object on success and and FBError object on error.
         */
        static Concurrency::task<FBResult^> FinishRequest(
            Platform::String^ uploadSessionId
            );
    };
}