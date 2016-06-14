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

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Foundation.Collections;
using winsdkfb;

namespace FBWinStoreCsTests
{
    class MockHttpClient : IHttpClient
    {
        public String ResponseData;

        public IAsyncOperation<string> DeleteTaskAsync(string path, PropertySet parameters)
        {
            return Task.Run(() =>
            {
                return ResponseData;
            }).AsAsyncOperation<string>();
        }

        public IAsyncOperation<string> GetTaskAsync(string path, PropertySet parameters)
        {
            return Task.Run(() =>
            {
                return ResponseData;
            }).AsAsyncOperation<string>();
        }

        public string ParametersToQueryString(PropertySet Parameters)
        {
            throw new NotImplementedException();
        }

        public IAsyncOperation<string> PostTaskAsync(string path, PropertySet parameters)
        {
            return Task.Run(() =>
            {
                return ResponseData;
            }).AsAsyncOperation<string>();
        }
    }
}
