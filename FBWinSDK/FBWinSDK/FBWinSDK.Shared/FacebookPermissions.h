#pragma once

namespace Facebook
{
	public ref class FBPermissions sealed
	{
		public:
            FBPermissions(
                Windows::Foundation::Collections::IVectorView<Platform::String^>^ Permissions
                );

			property Windows::Foundation::Collections::IVectorView<Platform::String^>^ Values
			{
				Windows::Foundation::Collections::IVectorView<Platform::String^>^ get();
			};

			virtual Platform::String^ ToString(
				) override;

		private:
			Windows::Foundation::Collections::IVectorView<Platform::String^>^ _values;
	};
}

