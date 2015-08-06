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
#include "FacebookResult.h"

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Facebook;

FBResult::FBResult(
    Platform::Object^ Object
    ) :
    _Object(nullptr),
    _Error(nullptr)
{
    _Error = dynamic_cast<FBError^>(Object);
    if (!_Error)
    {
        // Not an error, save as our object
        _Object = Object;
    }
}

FBResult::~FBResult(
    )
{
    OutputDebugString(L"FBResult destructor\n");
}

bool FBResult::Succeeded::get()
{
    return (_Object != nullptr);
}

Object^ FBResult::Object::get()
{
    return _Object;
}

FBError^ FBResult::ErrorInfo::get()
{
    return _Error;
}
