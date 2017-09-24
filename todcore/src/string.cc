#include "tod/string.h"
namespace tod
{

//-----------------------------------------------------------------------------
int String::hash() const
{
    unsigned int hash = 5381;
    auto str = this->c_str();
    for(auto i = 0u; i < this->size(); ++str, ++i)
        hash = ((hash << 5) + hash) + (*str);
    return hash;
}
    

//-----------------------------------------------------------------------------
String String::extractFileExtension() const
{
    auto p = this->rfind(S("."));
    if (String::npos == p) return String();
    return String(*this, p + 1, -1);
}


//-----------------------------------------------------------------------------
String String::extractPath() const
{
    auto p = this->rfind(S("/"));
    if (String::npos == p) return S("");
    return String(*this, 0, p);
}
    

//-----------------------------------------------------------------------------
String& String::lower()
{
    std::transform(this->begin(), this->end(), this->begin(), ::tolower);
    return *this;
}


//-----------------------------------------------------------------------------
String& String::upper()
{
    std::transform(this->begin(), this->end(), this->begin(), ::toupper);
    return *this;
}
    
    
//-----------------------------------------------------------------------------
int String::atoi(const char* str)
{
    int val = 0;
    bool neg = false;
    if (*str == '-')
    {
        neg = true;
        ++str;
    }
    while( *str )
    {
        val = val * 10 + (*str++ - '0');
    }
    return neg?-val:val;
}


//-----------------------------------------------------------------------------
template <>
double String::atof(const char* str)
{
    int frac;
    double sign, value, scale;

    // Skip leading white space, if any.
    while (std::isspace(*str)) ++str;

    // Get sign, if any.
    sign = 1.0;
    if (*str == S('-'))
    {
        sign = -1.0;
        ++str;
    }
    else if (*str == S('+'))
    {
        ++str;
    }

    // Get digits before decimal point or exponent, if any.
    for (value = 0.0; std::isdigit(*str); ++str)
    {
        value = value * 10.0 + (*str - S('0'));
    }

    // Get digits after decimal point, if any.    
    if (*str == '.')
    {
        double pow10 = 10.0;
        ++str;
        while (std::isdigit(*str))
        {
            value += (*str - S('0')) / pow10;
            pow10 *= 10.0;
            ++str;
        }
    }

    // Handle exponent, if any.    
    frac = 0;
    scale = 1.0;
    if ((*str == S('e')) || (*str == S('E')))
    {
        unsigned int expon;

        // Get sign of exponent, if any.
        ++str;
        if (*str == S('-'))
        {
            frac = 1;
            ++str;
        }
        else if (*str == S('+'))
        {
            ++str;
        }

        // Get digits of exponent, if any.
        for (expon = 0; std::isdigit(*str); ++str)
        {
            expon = expon * 10 + (*str - S('0'));
        }
        if (expon > 308) expon = 308;

        // Calculate scaling factor.        
        while (expon >= 50) { scale *= 1E50; expon -= 50; }
        while (expon >= 8) { scale *= 1E8;  expon -= 8; }
        while (expon >   0) { scale *= 10.0; expon -= 1; }
    }

    // Return signed and scaled floating point result.    
    return sign * (frac ? (value / scale) : (value * scale));
}


template <>
float String::atof<float>(const char* str)
{
    int frac;
    float sign, value, scale;

    // Skip leading white space, if any.
    while (std::isspace(*str)) ++str;

    // Get sign, if any.
    sign = 1.0;
    if (*str == S('-'))
    {
        sign = -1.0f;
        ++str;
    }
    else if (*str == S('+'))
    {
        ++str;
    }

    // Get digits before decimal point or exponent, if any.
    for (value = 0.0; std::isdigit(*str); ++str)
    {
        value = value * 10.0f + (*str - S('0'));
    }

    // Get digits after decimal point, if any.    
    if (*str == S('.'))
    {
        float pow10 = 10.0f;
        ++str;
        while (std::isdigit(*str))
        {
            value += (*str - S('0')) / pow10;
            pow10 *= 10.0f;
            ++str;
        }
    }

    // Handle exponent, if any.    
    frac = 0;
    scale = 1.0;
    if ((*str == S('e')) || (*str == S('E')))
    {
        unsigned int expon;

        // Get sign of exponent, if any.
        ++str;
        if (*str == S('-'))
        {
            frac = 1;
            ++str;
        }
        else if (*str == S('+'))
        {
            ++str;
        }

        // Get digits of exponent, if any.
        for (expon = 0; std::isdigit(*str); ++str)
        {
            expon = expon * 10 + (*str - S('0'));
        }
        if (expon > 308) expon = 308;

        // Calculate scaling factor.      
        while (expon >= 8) { scale *= 1E8;  expon -= 8; }
        while (expon >  0) { scale *= 10.0; expon -= 1; }
    }

    // Return signed and scaled floating point result.    
    return sign * (frac ? (value / scale) : (value * scale));
}

}
