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

#include <ppltasks.h>

#include "pch.h"
#include "FBVideoUploader.h"
#include "FacebookClient.h"
#include "FacebookSession.h"
#include "FacebookMediaObject.h"
#include "FacebookMediaStream.h"
#include "FBSingleValue.h"
#include "FBVideo.h"

using namespace concurrency;
using namespace winsdkfb;
using namespace winsdkfb::Graph;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::FileProperties;
using namespace Windows::Storage::Streams;
using namespace Windows::Security::Cryptography;
using namespace Windows::Security::Cryptography::DataProtection;

// Facebook Video upload doesn't specify whether their video threshold for small vs. large upload
// is 1 gigabyte or 1 gibibyte. We have put down the 1 gigabyte just to be safe.
#define VIDEO_THRESHOLD_SIZE 1000000000L

namespace winsdkfb
{
    IAsyncOperation<FBResult^>^ FBVideoUploader::UploadVideoAsync(StorageFile^ videoFile)
    {
        task<FBResult^> workTask = create_task(videoFile->GetBasicPropertiesAsync()).then([=](BasicProperties^ properties)
        {
            unsigned long long fileSize = properties->Size;
            FBSession^ sess = FBSession::ActiveSession;
            if (sess->LoggedIn)
            {
                // Facebook expects a file size of < 1 GB for small video upload and between 1 GB and 1.5 GB for large
                // video upload. We are only validating the file size portion before upload.
                if (fileSize < VIDEO_THRESHOLD_SIZE)
                {
                    return UploadSmallVideo(videoFile);
                }
                else
                {
                    return UploadLargeVideo(videoFile, fileSize);
                }
            }
            return create_task([=]() -> FBResult^
            {
                return ref new FBResult(ref new FBError(0, L"Not logged in", L"Need to be logged in to upload a video"));
            });
        });

        return create_async([=]()
        {
            return workTask;
        });
    }

    task<FBResult^> FBVideoUploader::UploadSmallVideo(StorageFile^ videoFile)
    {
        return create_task(videoFile->OpenReadAsync()).then([=](IRandomAccessStreamWithContentType^ fileStream)
        {
            FBMediaStream^ videoMediaStream = ref new FBMediaStream(videoFile->Name, fileStream);
            FBSession^ sess = FBSession::ActiveSession;
            PropertySet^ parameters = ref new PropertySet();
            parameters->Insert(L"title", videoFile->Name);
            parameters->Insert(L"source", videoMediaStream);

            String^ path = sess->User->Id + L"/videos";
            FBJsonClassFactory^ factory = ref new FBJsonClassFactory(FBVideo::FromJson);

            FBSingleValue^ request = ref new FBSingleValue(path, parameters, factory);
            return request->PostAsync();
        });
    }

    task<FBResult^> FBVideoUploader::UploadLargeVideo(StorageFile^ videoFile, unsigned long long fileSize)
    {
        return StartRequest(fileSize).then([=](FBResult^ result)
        {
            if (!result->Succeeded)
            {
                return create_task([=]()
                {
                    return result;
                });
            }
            else
            {
                FBVideo^ video = static_cast<FBVideo^>(result->Object);
                return create_task(videoFile->OpenAsync(FileAccessMode::Read)).then([=](IRandomAccessStream^ stream)
                {
                    FBVideoContentStream^ contentStream = ref new FBVideoContentStream(stream, videoFile->ContentType);
                    return std::make_pair(video, contentStream);
                }).then([=](std::pair<FBVideo^, FBVideoContentStream^> videoData)
                {
                    FBVideo^ video = videoData.first;
                    FBVideoContentStream^ stream = videoData.second;
                    return TransferRequest(stream, video, videoFile->Name);
                }).then([=](FBResult^ result) {
                    if (result->Succeeded)
                    {
                        FBVideo^ video = static_cast<FBVideo^>(result->Object);
                        return FinishRequest(video->UploadSessionId);
                    }
                    else
                    {
                        return create_task([=]()
                        {
                            return result;
                        });
                    }
                });
            }
        });
    }

    task<FBResult^> FBVideoUploader::StartRequest(unsigned long long fileSize)
    {
        PropertySet^ parameters = ref new PropertySet();
        parameters->Insert(L"upload_phase", L"start");
        parameters->Insert(L"file_size", fileSize.ToString());

        FBSession^ sess = FBSession::ActiveSession;
        String^ path = sess->User->Id + L"/videos";
        FBJsonClassFactory^ factory = ref new FBJsonClassFactory(FBVideo::FromJson);
        FBSingleValue^ request = ref new FBSingleValue(path, parameters, factory);
        return create_task(request->PostAsync());
    }

    task<FBResult^> FBVideoUploader::TransferRequest(
        FBVideoContentStream^ stream,
        FBVideo^ video,
        String^ title
    )
    {
        return create_task([=]()
        {
            FBJsonClassFactory^ factory = ref new FBJsonClassFactory(FBVideo::FromJson);
            int start = _wtoi(video->StartOffset->Data());
            int count = _wtoi(video->EndOffset->Data()) - start;
            FBResult^ result;
            while (count > 0)
            {
                task<FBResult^> workTask = create_task(stream->TruncateCloneStreamAsync(start, count)).then([=](FBVideoContentStream^ truncatedStream)
                {
                    FBMediaStream^ chunkMediaStream = ref new FBMediaStream(title, truncatedStream);
                    PropertySet^ parameters = ref new PropertySet();
                    parameters->Insert(L"upload_phase", L"transfer");
                    parameters->Insert(L"start_offset", start.ToString());
                    parameters->Insert(L"upload_session_id", video->UploadSessionId);
                    parameters->Insert(L"video_file_chunk", chunkMediaStream);

                    FBSession^ sess = FBSession::ActiveSession;
                    String^ path = sess->User->Id + L"/videos";
                    FBSingleValue^ request = ref new FBSingleValue(path, parameters, factory);
                    return request->PostAsync();
                });
                workTask.wait();
                result = workTask.get();
                if (result->Succeeded)
                {
                    FBVideo^ v = static_cast<FBVideo^>(result->Object);
                    // need to set UploadSessionId so that the next stage after
                    // the transfer can refer to it
                    v->UploadSessionId = video->UploadSessionId;
                    start = _wtoi(v->StartOffset->Data());
                    count = _wtoi(v->EndOffset->Data()) - start;
                }
                else
                {
                    break;
                }
            }
            if (count < 0)
            {
                result = ref new FBResult(
                    ref new FBError(
                        0,
                        L"Video upload error",
                        "transfer step reached an invalid byte range"));
            }
            return result;
        });
    }

    task<FBResult^> FBVideoUploader::FinishRequest(Platform::String^ uploadSessionId)
    {
        PropertySet^ parameters = ref new PropertySet();
        parameters->Insert(L"upload_phase", L"finish");
        parameters->Insert(L"upload_session_id", uploadSessionId);

        FBSession^ sess = FBSession::ActiveSession;
        String^ path = sess->User->Id + L"/videos";
        FBJsonClassFactory^ factory = ref new FBJsonClassFactory(FBVideo::FromJson);
        FBSingleValue^ request = ref new FBSingleValue(path, parameters, factory);
        return create_task(request->PostAsync());
    }
}
