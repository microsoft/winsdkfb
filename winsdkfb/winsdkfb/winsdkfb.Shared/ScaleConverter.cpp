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
#include "ScaleConverter.h"
#include <string>

using namespace winsdkfb;
using namespace Platform;
using namespace std;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Interop;

Object^ ScaleConverter::Convert(
    Object^ value,
    TypeName targetType,
    Object^ parameter,
    String^ language
    )
{
    if (targetType.Name != double::typeid->FullName)
    {
        throw ref new InvalidArgumentException();
    }

    double numValue = safe_cast<double>(value);

    String^ paramString = safe_cast<String^>(parameter);

    double numParam = stod(wstring(paramString->Data()));

    return numValue * numParam;
}
