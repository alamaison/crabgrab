/**
    @file

    Screenshotting.

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

#ifndef CRABGRAB_SCREENSHOT_HPP
#define CRABGRAB_SCREENSHOT_HPP

#include "crabgrab/convert_hbitmap.hpp" // convert_hbitmap_to_bmp

#include <winapi/error.hpp> // last_error

#include <boost/bind.hpp> // bind
#include <boost/exception/diagnostic_information.hpp> // diagnostic_information
#include <boost/exception/errinfo_api_function.hpp> // errinfo_api_function
#include <boost/exception/info.hpp> // errinfo
#include <boost/make_shared.hpp> // make_shared
#include <boost/shared_ptr.hpp> // shared_ptr
#include <boost/throw_exception.hpp> // BOOST_THROW_EXCEPTION
#include <boost/type_traits/remove_pointer.hpp> // remove_pointer

#include <stdexcept> // runtime_error
#include <vector>

#include <Windows.h> // GetWindowDC, ReleaseDC, CreateCompatibleDC, DeleteDC
                     // GetWindowRect, CreateCompatibleBitmap, DeleteObject,
                     // SelectObject, BitBlt

namespace crabgrab {

inline std::vector<unsigned char> take_screenshot(
    HWND hwnd=::GetDesktopWindow())
{
    boost::shared_ptr<boost::remove_pointer<HDC>::type> window_device_context(
        ::GetWindowDC(hwnd), boost::bind<int>(::ReleaseDC, hwnd, _1));
    if (!window_device_context)
        BOOST_THROW_EXCEPTION(
            boost::enable_error_info(
                std::runtime_error("Failed to get device context")) <<
            boost::errinfo_api_function("GetWindowDC"));

    boost::shared_ptr<boost::remove_pointer<HDC>::type> snapshot_device_context(
        ::CreateCompatibleDC(window_device_context.get()), ::DeleteDC);
    if (!snapshot_device_context)
        BOOST_THROW_EXCEPTION(
            boost::enable_error_info(
                std::runtime_error("Failed to create device context")) <<
            boost::errinfo_api_function("CreateCompatibleDC"));

    RECT r;
    ::GetWindowRect(hwnd, &r);

    boost::shared_ptr<boost::remove_pointer<HBITMAP>::type> snapshot(
        ::CreateCompatibleBitmap(
            window_device_context.get(), r.right - r.left, r.bottom - r.top),
            ::DeleteObject);
    if (!snapshot)
        BOOST_THROW_EXCEPTION(
            boost::enable_error_info(
                std::runtime_error("Failed to create bitmap")) <<
            boost::errinfo_api_function("CreateCompatibleBitmap"));

    HBITMAP orig = reinterpret_cast<HBITMAP>(
        ::SelectObject(snapshot_device_context.get(), snapshot.get()));
    if (!::BitBlt(
        snapshot_device_context.get(), 0, 0, r.right - r.left, 
        r.bottom - r.top, window_device_context.get(), 0, 0, SRCCOPY))
        BOOST_THROW_EXCEPTION(
            boost::enable_error_info(winapi::last_error()) <<
            boost::errinfo_api_function("BitBlt"));
    ::SelectObject(snapshot_device_context.get(), orig);

    return convert_hbitmap_to_bmp(
        snapshot.get(), snapshot_device_context.get());
}

}

#endif
