#include <ppltasks.h>

#include "pch.h"
#include "FacebookPermissions.h"

using namespace Facebook;
using namespace Platform;
using namespace Windows::Foundation::Collections;

IVectorView<String^>^ FBPermissions::Values::get()
{
	return _values;
};

void FBPermissions::Values::set(
	IVectorView<String^>^ value
	)
{
	_values = value;
}

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


