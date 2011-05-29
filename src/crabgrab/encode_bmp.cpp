/**
    @file

    Bitmap to PNG conversion

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

#include "crabgrab/encode_bmp.hpp"

#include <bitmap.h>
#include <LodePNG/lodepng.h>

#include <algorithm> // copy
#include <cassert> // assert
#include <sstream> // stringstream

namespace crabgrab {

namespace {

    void convert_row(
        unsigned char* row, size_t row_width,
        std::vector<unsigned char>& raw_out)
    {
        for (size_t i = 0; i < row_width; ++i)
        {
            if (i % 4 == 3)
                raw_out.push_back(0xff);
            else
                raw_out.push_back(row[i]);
        }
    }
}

std::vector<unsigned char> convert_bitmap_lines_to_raw(CBitmap& bitmap)
{
    std::vector<unsigned char> raw;

    size_t raw_byte_size;
    bitmap.GetBits(NULL, raw_byte_size);
 
    // total size should be exact multiple of height
    assert((raw_byte_size % bitmap.GetHeight()) == 0);

    size_t row_width = raw_byte_size / bitmap.GetHeight();

    // total size should be exact multiple of row size
    assert((raw_byte_size % row_width) == 0);

    unsigned char* start = reinterpret_cast<unsigned char*>(bitmap.GetBits());

    for (size_t h = bitmap.GetHeight(); h > 0; --h)
    {
        size_t row_offset = (h - 1) * row_width;
        convert_row(start + row_offset, row_width, raw);
    }

    return raw;
}

std::vector<unsigned char> encode_as_png(
    const std::vector<unsigned char>& bmp_bytes)
{
    std::stringstream stream;
    std::copy(
        bmp_bytes.begin(), bmp_bytes.end(),
        std::ostreambuf_iterator<char>(stream));

    CBitmap bitmap;
    stream.seekg(0);
    bitmap.Load(stream);

    std::vector<unsigned char> png_out;
    LodePNG::Encoder encoder;
    encoder.getSettings().autoLeaveOutAlphaChannel = 1;
    encoder.encode(
        png_out, convert_bitmap_lines_to_raw(bitmap), bitmap.GetWidth(),
        bitmap.GetHeight());
    if(encoder.hasError())
    {
        std::cout << "Encoder error " << encoder.getError() << ": " <<
            LodePNG_error_text(encoder.getError()) << std::endl;
        return std::vector<unsigned char>();
    }

    return png_out;
}

}
