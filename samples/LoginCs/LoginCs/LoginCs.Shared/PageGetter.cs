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

using Facebook;
using Facebook.Graph;

namespace LoginCs
{
    class PageGetter 
    {
        public PageGetter()
        {
        }

        public void HandleError(
            FBError err
            )
        {
            //Display error message here
        }

        public async void GetPageWithId(
            string ID
            )
        {
            FBSingleValue val = new FBSingleValue("/" + ID, null,
                new FBJsonClassFactory((JsonText) => 
                    MyFBPage.FromJson(JsonText)));
            FBResult result = await val.GetAsync();
            if (result.Succeeded)
            {
                MyFBPage page = (MyFBPage)result.Object;
                //Do something with page here
            }
            else
            {
                HandleError(result.ErrorInfo);
            }
        }
    }
}
