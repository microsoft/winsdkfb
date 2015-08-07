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


