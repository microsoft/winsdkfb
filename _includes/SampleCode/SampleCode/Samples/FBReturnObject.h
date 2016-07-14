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

        property Platform::String^ Post_Id
        {
            Platform::String^ get()
            {
                return _post_id;
            }
            void set(Platform::String^ value)
            {
                _post_id = value;
            }
        }

    private:
        Platform::String^ _id;
        Platform::String^ _post_id;
    };
}
