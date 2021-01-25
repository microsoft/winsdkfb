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
#include "GraphUriBuilder.h"
#include "FacebookSession.h"

#include <regex>
#include <sstream>

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace winsdkfb;


GraphUriBuilder::GraphUriBuilder(String^ path, String^ graphDomain)
    :
    _queryParams { ref new PropertySet() }
{
    Uri^ testUri;
    bool buildDomain = false;
    try
    {
        testUri = ref new Uri(path);
    }
    catch (Platform::InvalidArgumentException^ e)
    {
        buildDomain = true;
    }

    if (buildDomain)
    {
        String^ domain = L"https://graph.facebook.com/";
		if (graphDomain == L"gaming")
		{
			domain = L"https://graph.fb.gg/";
		}
        testUri = ref new Uri(domain + path);
    }
    _host = testUri->Host;
    _path = testUri->Path;
    _scheme = testUri->SchemeName;

    // remove any extra '/'s from path
    FixPathDelimiters();
    // capture api version from path or build it
    BuildApiVersionString();
    // save query params in map for later use
    DecodeQueryParams(testUri);
}

void GraphUriBuilder::AddQueryParam(Platform::String^ query, Platform::String^ param)
{
    _queryParams->Insert(query, param);
}

Windows::Foundation::Uri^ GraphUriBuilder::MakeUri()
{
    // this request_host stuff is a bit of a hack to send graph calls in
    // the unit tests to our own web service.
    if (_queryParams->HasKey(L"request_host"))
    {
        _host = static_cast<String^>(_queryParams->Lookup(L"request_host"));
    }
    String^ fullPath = _scheme + L"://" + _host + L"/" + _apiVersion + _path;

    if (_queryParams->Size > 0)
    {
        String^ separator = L"?";
        auto it = _queryParams->First();
        while (it->HasCurrent)
        {
            fullPath += separator + Uri::EscapeComponent(it->Current->Key) + L"=" + Uri::EscapeComponent(static_cast<String^>(it->Current->Value));
            separator = L"&";
            it->MoveNext();
        }
    }
    return ref new Uri(fullPath);
}

void GraphUriBuilder::BuildApiVersionString()
{
    std::wstring regexString = LR"__(^.?(v\d\.\d)(.*))__";
    std::wregex apiRegex{ regexString };
    std::wsmatch match;
    std::wstring searchString{ _path->Data() };
    std::regex_match(searchString, match, apiRegex);
    if (match.size() >= 3) // 1 for matched string + 2 for each capture group
    {
        std::wstring apiString = match[1].str();
        _apiVersion = ref new String(apiString.c_str());
        // need to adjust _path so that the api version doesn't get added twice
        std::wstring newPath = match[2].str();
        _path = ref new String(newPath.c_str());
    }
    else
    {
        FBSession^ sess = FBSession::ActiveSession;
        if (sess->APIMajorVersion)
        {
            _apiVersion = "v" + sess->APIMajorVersion.ToString() + "." + sess->APIMinorVersion.ToString();
        }
    }
}

void GraphUriBuilder::FixPathDelimiters()
{
    String^ fixedPath = L"";
    std::wstring originalPath{ _path->Data() };
    std::wistringstream iss{ originalPath };
    std::wstring token;
    while (std::getline(iss, token, L'/'))
    {
        if (token.size() > 0)
        {
            fixedPath += L"/" + ref new String(token.c_str());
        }
    }
    _path = fixedPath;
}

void GraphUriBuilder::DecodeQueryParams(Uri^ uri)
{
    WwwFormUrlDecoder^ decoder = uri->QueryParsed;
    if (decoder->Size > 0)
    {
        IIterator<IWwwFormUrlDecoderEntry^>^ it = decoder->First();
        while (it->HasCurrent)
        {
            IWwwFormUrlDecoderEntry^ current = it->Current;
            _queryParams->Insert(current->Name, current->Value);
            it->MoveNext();
        }
    }
}