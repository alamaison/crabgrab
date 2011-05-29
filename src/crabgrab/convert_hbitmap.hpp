/**
    @file

    Extract BMP (DIB file) from HBITMAP.

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

#ifndef CRAB_CONVERT_HBITMAP_HPP
#define CRAB_CONVERT_HBITMAP_HPP

#include <boost/filesystem/fstream.hpp> // ofstream
#include <boost/filesystem/path.hpp> // path
#include <boost/numeric/conversion/cast.hpp> // numeric_cast
#include <boost/shared_ptr.hpp> // shared_ptr
#include <boost/throw_exception.hpp> // BOOST_THROW_EXCEPTION

#include <cassert> // assert
#include <cstdlib> // malloc, free
#include <cstring> // memcpy, memset
#include <utility> // make_pair
#include <vector>

#include <Windows.h>

namespace crabgrab {
namespace detail {

inline BITMAPINFOHEADER dib_information(HBITMAP bitmap, HDC device_context)
{
    BITMAPINFO info = BITMAPINFO();
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

    int rc = ::GetDIBits(
        device_context, bitmap, 0, 0, NULL, &info, DIB_RGB_COLORS);
    if (rc == 0)
        BOOST_THROW_EXCEPTION(
            std::exception(
                "Could not get device-independent bitmap attributes from "
                "the bitmap"));

    return info.bmiHeader;
}

inline size_t calculate_colour_table_size(const BITMAPINFOHEADER& header)
{
    switch (header.biBitCount)
    {
        // BMPs with fewer than 16bpp have 2^n table entries where n is
        // the bpp
    case 1:
    case 4:
    case 8:
        return sizeof(RGBQUAD) * (1 << header.biBitCount);

        // BMPs with more than 8bpp don't have to have a colour table but,
        // if they do, the number of entries in it is given in the header
    case 16:
    case 24:
    case 32:
        return sizeof(RGBQUAD) * header.biClrUsed;
    default:
        BOOST_THROW_EXCEPTION(
            std::exception("Invalid number of bits per pixel"));
    }
}

/**
 * @todo Support BI_ALPHABITFIELDS.
 */
inline size_t calculate_header_and_colour_table_size(
    const BITMAPINFOHEADER& header)
{
    size_t size = sizeof(BITMAPINFOHEADER);
    
    if (header.biCompression == BI_BITFIELDS)
        size += 3 * sizeof(DWORD);
    else
        size += calculate_colour_table_size(header);

    return size;
}

/**
 * Create DIB header with empty colour table.
 *
 * Returned header is ready for a call to GetDIBits with a buffer.  Calling it
 * this way requires space for the colour table to have been allocated.
 *
 * @todo Replace pair with a DIB+colour-table class.
 */
inline std::pair<boost::shared_ptr<BITMAPINFO>, size_t> dib_header_from_bitmap(
    HBITMAP bitmap, HDC device_context)
{
    // This returns the header but it doesn't include the colour table which
    // can vary in size (or may not even exist).  However, if we want to use
    // GetDIBits to get the actual bitmap data, the header we pass it must
    // include extra bytes for the colour table if there is one.  Therefore,
    // we copy it to larger blob of memory and pretend it's a BITMAPINFOHEADER.
    // In fact, we pretend it's a BITMAPINFO because that is how GetDIBits
    // expects to receive it.
    BITMAPINFOHEADER header = dib_information(bitmap, device_context);

    size_t actual_blob_size = calculate_header_and_colour_table_size(header);

    assert(actual_blob_size >= sizeof(BITMAPINFOHEADER));

    // We allocate the blob using malloc because we want a specific size, not
    // a specific type.
    boost::shared_ptr<BITMAPINFO> p(
        reinterpret_cast<BITMAPINFO*>(std::malloc(actual_blob_size)),
        std::free);

    std::memcpy(&p->bmiHeader, &header, sizeof(header));
    p->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

    if (actual_blob_size > sizeof(header))
        std::memset(&p->bmiColors, 0, actual_blob_size - sizeof(header));

    return std::make_pair(p, actual_blob_size);
}

inline BITMAPFILEHEADER file_header_for_bitmap(
    const BITMAPINFOHEADER& dib_header, size_t dib_header_and_colour_table_size)
{
    BITMAPFILEHEADER file_header = BITMAPFILEHEADER();
    size_t total_header_size =
        sizeof(BITMAPFILEHEADER) + dib_header_and_colour_table_size;

    file_header.bfType = 'MB';
    file_header.bfSize =
        boost::numeric_cast<DWORD>(total_header_size + dib_header.biSizeImage);
    file_header.bfOffBits = boost::numeric_cast<DWORD>(total_header_size);

    return file_header;
}

inline unsigned char* write_file_header(
    unsigned char* p, const BITMAPFILEHEADER& file_header)
{
    std::memcpy(p, &file_header, sizeof(file_header));
    return p + sizeof(file_header);
}

inline unsigned char* write_dib_header_and_colour_table(
    unsigned char* p,
    std::pair<boost::shared_ptr<BITMAPINFO>, size_t> dib_header)
{
    std::memcpy(p, dib_header.first.get(), dib_header.second);
    return p + dib_header.second;
}

}

inline std::vector<unsigned char> convert_hbitmap_to_bmp(
    HBITMAP bitmap, HDC device_context)
{
    std::pair<boost::shared_ptr<BITMAPINFO>, size_t> dib_header =
        detail::dib_header_from_bitmap(bitmap, device_context);

    size_t total_header_size = sizeof(BITMAPFILEHEADER) + dib_header.second;
    size_t bmp_file_size =
        total_header_size + dib_header.first->bmiHeader.biSizeImage;

    std::vector<unsigned char> buffer(bmp_file_size);

    // Must not modify dib_header before this call or the size calculation will
    // be wrong for what comes out of the second call
    int rc = ::GetDIBits(
        device_context, bitmap, 0, dib_header.first->bmiHeader.biHeight,
        &buffer[0] + total_header_size, dib_header.first.get(), DIB_RGB_COLORS);
    if (rc == 0)
        BOOST_THROW_EXCEPTION(
            std::exception("Failed to convert bitmap to DIB format"));

    unsigned char* p = &buffer[0];

    BITMAPFILEHEADER file_header = detail::file_header_for_bitmap(
        dib_header.first->bmiHeader, dib_header.second);
    p = detail::write_file_header(p, file_header);

    p = detail::write_dib_header_and_colour_table(p, dib_header);

    return buffer;
}

}

#endif
