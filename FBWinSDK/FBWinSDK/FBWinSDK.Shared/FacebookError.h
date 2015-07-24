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
    /*!
     * \brief error codes.
     */
    public enum class ErrorCode : int
    {
        ErrorCodeOauthException = 190
    };

    /*!
    * \brief error subcodes.
    */
    public enum class ErrorSubcode : int
    {
        ErrorSubcodeSessionInvalidated = 466
    };

    /*!
     * \brief class wrapper for FB error responses. For the time being, this is 
     * a very bare shell around the error response URL from Facebook, and not a
     * full-blown error class such as exists in the Facebook SDK for iOS.
     */
    public ref class FBError sealed
    {
        public:
            static FBError^ FromUri(
                Windows::Foundation::Uri^ ResponseUri
                );

            static FBError^ FromJson(
                Platform::String^ JsonText
                );

            FBError(
                int Code,
                Platform::String^ Type,
                Platform::String^ Message
                );

            //! Error message string
            property Platform::String^ Message
            {
                Platform::String^ get();
            }

            //! Error type string
            property Platform::String^ Type
            {
                Platform::String^ get();
            }

            //! Error code
            property int Code
            {
                int get();
            }

            //! Error subcode
            property int Subcode
            {
                int get();
            }

            //! Error user title
            property Platform::String^ ErrorUserTitle
            {
                Platform::String^ get();
            }

            //! Error user message
            property Platform::String^ ErrorUserMessage
            {
                Platform::String^ get();
            }

        private:
            FBError(
                );

            Platform::String^ m_message;
            Platform::String^ m_type;
            int     m_code;
            int     m_subcode;
            Platform::String^ m_errorUserTitle;
            Platform::String^ m_errorUserMessage;
    };
}