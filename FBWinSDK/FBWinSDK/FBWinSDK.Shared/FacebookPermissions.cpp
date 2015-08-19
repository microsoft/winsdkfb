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
#include <ppltasks.h>

#include "pch.h"
#include "FacebookPermissions.h"

using namespace Facebook;
using namespace Platform;
using namespace Windows::Foundation::Collections;

FBPermissions::FBPermissions(
    IVectorView<String^>^ Permissions
    )
{
    _values = Permissions;
}

IVectorView<String^>^ FBPermissions::Values::get()
{
	return _values;
};

Platform::String^ FBPermissions::ToString(
	)
{
	String^ permissions = nullptr;
	if (_values)
	{
		permissions = ref new String();

		for (unsigned int i = 0; i < _values->Size; i++)
		{
			if (i)
			{
				permissions += ",";
			}

			permissions += _values->GetAt(i);
		}
	}

	return permissions;
}


