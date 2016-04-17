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
#include "HttpManager.h"
#include "FacebookClient.h"

using namespace winsdkfb;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Platform;

HttpManager^ HttpManager::Instance::get()
{
    static HttpManager^ _instance = ref new HttpManager(ref new FBClient());
    return _instance;
}

void HttpManager::SetHttpClient(IHttpClient^ httpClient)
{
    _httpClient = httpClient;
}

IAsyncOperation<String^>^ HttpManager::GetTaskAsync(String^ path, PropertySet^ parameters)
{
    return _httpClient->GetTaskAsync(path, parameters);
}

IAsyncOperation<String^>^ HttpManager::PostTaskAsync(String^ path, PropertySet^ parameters)
{
    return _httpClient->PostTaskAsync(path, parameters);
}

IAsyncOperation<String^>^ HttpManager::DeleteTaskAsync(String^ path, PropertySet^ parameters)
{
    return _httpClient->DeleteTaskAsync(path, parameters);
}

String^ HttpManager::ParametersToQueryString(
    PropertySet^ Parameters
    )
{
    return _httpClient->ParametersToQueryString(Parameters);
}


HttpManager::HttpManager(IHttpClient^ httpClient)
{
    _httpClient = httpClient;
}