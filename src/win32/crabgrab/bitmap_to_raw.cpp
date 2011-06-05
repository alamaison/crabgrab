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

#include "crabgrab/bitmap_to_raw.hpp"

#include <bitmap.h> // CBitmap

#include <algorithm> // copy
#include <cassert> // assert
#include <sstream> // stringstream

using std::ostreambuf_iterator;
using std::stringstream;
using std::uint8_t;
using std::vector;

namespace crabgrab {

namespace {

void convert_row(uint8_t* row, size_t row_width, vector<uint8_t>& raw_out)
{
    for (size_t i = 0; i < row_width; ++i)
    {
        if (i % 4 == 3)
            raw_out.push_back(0xff);
        else
            raw_out.push_back(row[i]);
    }
}

vector<uint8_t> convert_bitmap_lines_to_raw(CBitmap& bitmap)
{
    vector<uint8_t> raw;

    size_t raw_byte_size;
    bitmap.GetBits(NULL, raw_byte_size);
 
    // total size should be exact multiple of height
    assert((raw_byte_size % bitmap.GetHeight()) == 0);

    size_t row_width = raw_byte_size / bitmap.GetHeight();

    // total size should be exact multiple of row size
    assert((raw_byte_size % row_width) == 0);

    uint8_t* start = reinterpret_cast<uint8_t*>(bitmap.GetBits());

    for (size_t h = bitmap.GetHeight(); h > 0; --h)
    {
        size_t row_offset = (h - 1) * row_width;
        convert_row(start + row_offset, row_width, raw);
    }

    return raw;
}

}

raw_image convert_bitmap_to_raw(const vector<uint8_t>& bmp_bytes)
{
    stringstream stream;
    copy(
        bmp_bytes.begin(), bmp_bytes.end(), ostreambuf_iterator<char>(stream));

    CBitmap bitmap;
    stream.seekg(0);
    bitmap.Load(stream);

    return raw_image(
        bitmap.GetWidth(), bitmap.GetHeight(),
        convert_bitmap_lines_to_raw(bitmap));
}

}
