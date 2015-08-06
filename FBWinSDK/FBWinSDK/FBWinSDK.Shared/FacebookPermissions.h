#pragma once

namespace Facebook
{
	public ref class FBPermissions sealed
	{
		public:

			property Windows::Foundation::Collections::IVectorView<Platform::String^>^ Values
			{
				Windows::Foundation::Collections::IVectorView<Platform::String^>^ get();
				void set(Windows::Foundation::Collections::IVectorView<Platform::String^>^ value);
			};

			virtual Platform::String^ ToString(
				) override;

		private:
			Windows::Foundation::Collections::IVectorView<Platform::String^>^ _values;
	};
}

