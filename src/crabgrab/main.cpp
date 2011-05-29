/**
    @file

    Main crabgrab loop.

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

#include "crabgrab/clipboard.hpp" // put_clipboard_text
#include "crabgrab/convert_hbitmap.hpp"
#include "crabgrab/encode_bmp.hpp"
#include "crabgrab/twitpic/response.hpp" // handle_response
#include "crabgrab/twitpic/twitpic.hpp"

#include <winapi/error.hpp> // last_error
#include <winapi/hook.hpp> // windows_hook

#include <boost/bind.hpp> // bind
#include <boost/exception/diagnostic_information.hpp> // diagnostic_information
#include <boost/shared_ptr.hpp> // shared_ptr
#include <boost/throw_exception.hpp> // BOOST_THROW_EXCEPTION
#include <boost/type_traits/remove_pointer.hpp> // remove_pointer

#include <iostream> // cout, cin, cerr
#include <string>

#include <Windows.h>
#include <tchar.h>

namespace crabgrab {

void grab_window(HWND hwnd)
{
    boost::shared_ptr< boost::remove_pointer<HDC>::type > window_device_context(
        ::GetWindowDC(hwnd), boost::bind<int>(ReleaseDC, hwnd, _1));
    if (!window_device_context)
        BOOST_THROW_EXCEPTION(std::exception("Failed to get device context"));

    boost::shared_ptr< boost::remove_pointer<HDC>::type > snapshot_device_context(
        ::CreateCompatibleDC(window_device_context.get()), DeleteDC);
    if (!snapshot_device_context)
        BOOST_THROW_EXCEPTION(
            std::exception("Failed to create device context"));

    RECT r;
    ::GetWindowRect(hwnd, &r);

    boost::shared_ptr< boost::remove_pointer<HBITMAP>::type > snapshot(
        ::CreateCompatibleBitmap(
            window_device_context.get(), r.right - r.left, r.bottom - r.top),
            ::DeleteObject);
    if (!snapshot)
        BOOST_THROW_EXCEPTION(std::exception("Failed to create bitmap"));

    HBITMAP orig = reinterpret_cast<HBITMAP>(
        ::SelectObject(snapshot_device_context.get(), snapshot.get()));
    if (!::BitBlt(
        snapshot_device_context.get(), 0, 0, r.right - r.left, 
        r.bottom - r.top, window_device_context.get(), 0, 0, SRCCOPY))
        BOOST_THROW_EXCEPTION(winapi::last_error());
    ::SelectObject(snapshot_device_context.get(), orig);

    std::vector<unsigned char> bmp = convert_hbitmap_to_bmp(
        snapshot.get(), snapshot_device_context.get());

    std::cout << "TwitPic username: ";
    std::string username;
    std::cin >> username;

    std::cout << "Password (will be shown here!): ";
    std::string password;
    std::cin >> password;

    std::string xml_response = twitpic::upload_image(
        username, password, encode_as_png(bmp));

    std::string url;
    try
    {
        url = twitpic::handle_response(xml_response);
    }
    catch (const twitpic::twitpic_exception& e)
    {
        std::cerr << "FAILED: " << e.what() << std::endl;
        return;
    }

    try
    {
        put_clipboard_text(url);
    }
    catch (const std::exception& e)
    {
        std::cerr << "CLIPBOARD FAILURE: " << e.what() << std::endl;
        std::cout <<
            "Crabgrab couldn't put the link to your screenshot onto the "
            "clipboard to here it is instead: " << url << std::endl;
        return;
    }

    std::cout <<
        "The link to your screenshot is on the clipboard." << std::endl;
}


class window_hook_procedure
{
public:
    virtual ~window_hook_procedure() {}

    LRESULT call_next_hook(
        int nCode, WPARAM wParam, LPARAM lParam)
    {
        return ::CallNextHookEx(NULL /*hhook*/, nCode, wParam, lParam);
    }

};

class low_level_keyboard_hook_procedure : public window_hook_procedure
{
private:
    virtual bool operator()(int message, KBDLLHOOKSTRUCT& input_event) = 0;

public:

    LRESULT CALLBACK hook_proc(int code, WPARAM wparam, LPARAM lparam)
    {
        if (code < HC_ACTION)
            return call_next_hook(code, wparam, lparam);

        if ((*this)(wparam, *reinterpret_cast<KBDLLHOOKSTRUCT*>(lparam)))
            return TRUE; // handled

        return call_next_hook(code, wparam, lparam);
    }
};

class crabgrab_keyboard_hook : public low_level_keyboard_hook_procedure
{
public:
    crabgrab_keyboard_hook() : m_is_shift_pressed(false) {}

private:
    virtual bool operator()(int message, KBDLLHOOKSTRUCT& input_event)
    {
        switch (message)
        {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            switch (input_event.vkCode)
            {
            case VK_LSHIFT:
            case VK_RSHIFT:
                {
                    m_is_shift_pressed = true;
                    return false;
                }
            case VK_SNAPSHOT:
                {
                    HWND hwnd = (m_is_shift_pressed) ?
                        ::GetForegroundWindow() : ::GetDesktopWindow();

                    try
                    {
                        grab_window(hwnd);
                    }
                    catch (const std::exception& e)
                    {
                        std::cout << e.what();
                    }

                    return true;
                }
            }

            break;

        case WM_KEYUP:
        case WM_SYSKEYUP:
            switch (input_event.vkCode)
            {
            case VK_LSHIFT:
            case VK_RSHIFT:
                {
                    m_is_shift_pressed = false;
                    return false;
                }
            }

            break;
        }

        return false;
    }

    bool m_is_shift_pressed;
};

LRESULT CALLBACK hook_proc(int code, WPARAM wparam, LPARAM lparam)
{
    static crabgrab_keyboard_hook hook;
    return hook.hook_proc(code, wparam, lparam);
}

void run()
{
    winapi::hhook hhook = winapi::windows_global_hook(WH_KEYBOARD_LL, &hook_proc);

    MSG message;
    while (::GetMessage(&message, NULL, 0, 0) != 0)
    {
        ::TranslateMessage(&message);
        ::DispatchMessage(&message);
    }
}

}

int _tmain(int argc, _TCHAR* argv[])
{
    try
    {
        crabgrab::run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Unhandled exception:" << std::endl;
        std::cerr << boost::diagnostic_information(e);
    }

    return 0;
}

