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
#include "HlsColor.h"

using namespace Facebook;
using namespace Platform;
using namespace std;
using namespace Windows::UI;


Color HlsColor::RgbValue::get(
    )
{
    Color result;

    if (_S == 0)
    {
        result.R = (byte)(_L * 255);
        result.G = (byte)(_L * 255);
        result.B = (byte)(_L * 255);
        result.A = (byte)(_A * 255);
    }
    else
    {
        double t1;
        if (_L < 0.5)
        {
            t1 = _L * (1.0 + _S);
        }
        else
        {
            t1 = _L + _S - (_L * _S);
        }

        double t2 = (2.0 * _L) - t1;

        double h = _H / 360;

        double tR = h + (1.0 / 3.0);
        double r = TransformColor(t1, t2, tR);

        double tG = h;
        double g = TransformColor(t1, t2, tG);

        double tB = h - (1.0 / 3.0);
        double b = TransformColor(t1, t2, tB);

        result.R = (byte)(r * 255);
        result.G = (byte)(g * 255);
        result.B = (byte)(b * 255);
        result.A = (byte)(_A * 255);
    }

    return result;
}

void HlsColor::RgbValue::set(
    Color value
    )
{
    double red = value.R / 255.0;
    double green = value.G / 255.0;
    double blue = value.B / 255.0;
    double alpha = value.A / 255.0;

    double smallest = min(red, min(green, blue));
    double largest  = max(red, max(green, blue));
    double delta = largest - smallest;

    _A = alpha;

    if (largest == smallest)
    {
        _H = 0;
        _S = 0;
        _L = largest;
    }
    else
    {
        _L = (smallest + largest) / 2.0;
        if (_L < 0.5)
        {
            _S = delta / (largest + smallest);
        }
        else
        {
            _S = delta / (2.0 - largest - smallest);
        }

        if (red == largest)
        {
            _H = (green - blue) / delta;
        }
        else if (green == largest)
        {
            _H = 2.0 + ((blue - red) / delta);
        }
        else
        {
            _H = 4.0 + ((red - green) / delta);
        }

        _H *= 60;
        if (_H < 0)
        {
            _H += 360;
        }
    }
}

double HlsColor::Luminosity::get(
    )
{
    return _L;
}

void HlsColor::Luminosity::set(
    double value
    )
{
    _L = value;
}

double HlsColor::TransformColor(
    double t1,
    double t2,
    double t3
    )
{
    if (t3 < 0)
    {
        t3 += 1.0;
    }

    if (t3 > 1)
    {
        t3 -= 1.0;
    }

    double color;
    if (6.0 * t3 < 1)
    {
        color = t2 + ((t1 - t2) * 6.0 * t3);
    }
    else if (2.0 * t3 < 1)
    {
        color = t1;
    }
    else if (3.0 * t3 < 2)
    {
        color = t2 + ((t1 - t2) * ((2.0 / 3.0) - t3) * 6.0);
    }
    else
    {
        color = t2;
    }

    return color;
}
