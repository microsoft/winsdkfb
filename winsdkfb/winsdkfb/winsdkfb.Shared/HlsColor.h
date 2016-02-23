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

namespace winsdkfb
{
    ref class HlsColor sealed
    {
    public:
        property Windows::UI::Color RgbValue
        {
            Windows::UI::Color get();
            void set(Windows::UI::Color value);
        }

        property double Luminosity
        {
            double get();
            void set(double value);
        }

    private:
        double TransformColor(
            double t1,
            double t2,
            double t3
            );

        double _A;
        double _H;
        double _L;
        double _S;
    };
}