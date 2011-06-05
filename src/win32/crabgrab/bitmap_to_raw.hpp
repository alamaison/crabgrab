/**
    @file

    Bitmap to raw pixel data conversion.

    @if license

    Copyright (C) 2011  Alexander Lamaison <awl03@doc.ic.ac.uk>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    If you modify this Program, or any covered work, by linking or
    combining it with the OpenSSL project's OpenSSL library (or a
    modified version of that library), containing parts covered by the
    terms of the OpenSSL or SSLeay licenses, the licensors of this
    Program grant you additional permission to convey the resulting work.

    @endif
*/

#ifndef CRABGRAB_WIN32_CRABGRAB_BITMAP_TO_RAW
#define CRABGRAB_WIN32_CRABGRAB_BITMAP_TO_RAW

#include <cstdint> // uint8_t
#include <vector>

namespace crabgrab {

class raw_image
{
public:
    raw_image(
        size_t width, size_t height,
        const std::vector<std::uint8_t>& pixel_data) 
        : m_width(width), m_height(height), m_pixel_data(pixel_data) {}

    size_t width() const { return m_width; }

    size_t height() const { return m_height; }

    const std::vector<std::uint8_t>& pixel_data() const
    {
        return m_pixel_data;
    }

private:
    size_t m_width;
    size_t m_height;
    std::vector<std::uint8_t> m_pixel_data;
};

raw_image convert_bitmap_to_raw(const std::vector<std::uint8_t>& bmp_bytes);

}

#endif