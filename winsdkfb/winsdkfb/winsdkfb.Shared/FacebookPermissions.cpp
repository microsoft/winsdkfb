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
#include <sstream>

using namespace winsdkfb;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace std;

FBPermissions::FBPermissions(
    IVectorView<String^>^ Permissions
    )
{
    _values = Permissions;
}

FBPermissions^ FBPermissions::FromString(
    String^ Permissions
    )
{
    return ref new FBPermissions(ParsePermissionsFromString(Permissions));
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

IVectorView<String^>^ FBPermissions::ParsePermissionsFromString(
    String^ Permissions
    )
{
    const int bufferSize = 64;
    wstring wstringPermissions = Permissions->Data();
    wstringstream wss{wstringPermissions};
    Vector<String^>^ parsedPermissions = ref new Vector<String^>();
    wchar_t temp[bufferSize];
    while (true)
    {
        if (wss.eof())
        {
            break;
        }
        wss.getline(temp, bufferSize, L',');
        parsedPermissions->Append(ref new String(temp));
    }
    return parsedPermissions->GetView();
}

FBPermissions^ FBPermissions::Difference(
    FBPermissions^ Minuend,
    FBPermissions^ Subtrahend
    )
{
    Vector<String^>^ remainingPermissions = ref new Vector<String^>();
    // stick each permissions into vector manually since copy constructor won't work with IVectorView
    for (String^ perm : Minuend->Values)
    {
        remainingPermissions->Append(perm);
    }
    for (String^ otherPerm : Subtrahend->Values)
    {
        for (int i = 0; i < remainingPermissions->Size; ++i)
        {
            String^ perm = remainingPermissions->GetAt(i);
            if (String::CompareOrdinal(perm, otherPerm) == 0)
            {
                remainingPermissions->RemoveAt(i);
                break;
            }
        }
    }
    return ref new FBPermissions(remainingPermissions->GetView());
}
