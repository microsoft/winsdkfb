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

namespace SDKCppUnit
{
    public delegate void TestSuiteStartedHandler(
        Platform::String^ SuiteName
        );

    public delegate void TestSuiteCompletedHandler(
        Platform::String^ SuiteName
        );

    public delegate void TestCaseStartedHandler(
        Platform::String^ CaseName
        );

    public delegate void TestCaseCompletedHandler(
        Platform::String^ CaseName,
        bool TestResult
        );

    ref class TestRunner sealed
    {
    public:
        TestRunner();

        void RunAllTests();

        event TestSuiteStartedHandler^ TestSuiteStarted;
        event TestSuiteCompletedHandler^ TestSuiteCompleted;
        event TestCaseStartedHandler^ TestCaseStarted;
        event TestCaseCompletedHandler^ TestCaseCompleted;

    private:
        void NotifyTestSuiteStarted(
            Platform::String^ SuiteName
            );

        void NotifyTestSuiteCompleted(
            Platform::String^ SuiteName
            );

        void NotifyTestCaseStarted(
            Platform::String^ CaseName
            );

        void NotifyTestCaseCompleted(
            Platform::String^ CaseName,
            bool TestResult
            );

        Windows::Foundation::IAsyncOperation
            <Windows::Storage::Streams::IRandomAccessStreamWithContentType^>^
        GetStreamOnFileAsync(
            Platform::String^ path
            );

        Windows::Foundation::IAsyncOperation<Facebook::FBResult^>^ 
        TestRunner::UploadPhotoFromStreamAsync(
            Platform::String^ Path,
            Windows::Storage::Streams::IRandomAccessStreamWithContentType^ Stream,
            Windows::Foundation::Collections::PropertySet^ Parameters
            );

        Windows::Foundation::IAsyncOperation<Facebook::FBResult^>^ 
        TestRunner::GetExtendedPhotoInfoForAsync(
            Facebook::FBResult^ Result,
            Windows::Foundation::Collections::PropertySet^ Parameters
            );

        Windows::Foundation::IAsyncOperation<Windows::Storage::Streams::IInputStream^>^ 
        TestRunner::DownloadPhotoAsync(
            Facebook::FBResult^ Result,
            Windows::Foundation::Collections::PropertySet^ Parameters
            );

        concurrency::task<bool> TestCreateAppToken(
            );

        concurrency::task<bool> TestCreateTestUser(
            );

        concurrency::task<bool> TestDeleteAllTestUsers(
            );

        concurrency::task<bool> TestUploadPhoto(
            );
    };
};