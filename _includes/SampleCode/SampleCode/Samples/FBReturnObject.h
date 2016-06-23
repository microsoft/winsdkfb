#pragma once

namespace SampleCode
{
    public ref class FBReturnObject sealed
    {
    public:
        property Platform::String^ Id
        {
            Platform::String^ get()
            {
                return _id;
            }
            void set(Platform::String^ value)
            {
                _id = value;
            }
        }
    private:
        Platform::String^ _id;
    };
}
