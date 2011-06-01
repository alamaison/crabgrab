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
#include "crabgrab/encode_bmp.hpp"
#include "crabgrab/screenshot.hpp" // take_screenshot
#include "crabgrab/notification.hpp" // notification_icon
#include "crabgrab/twitpic/response.hpp" // handle_response
#include "crabgrab/twitpic/twitpic.hpp"

#include <winapi/dynamic_link.hpp> // module_handle
#include <winapi/gui/icon.hpp> // load_icon
#include <winapi/hook.hpp> // windows_hook

#include <boost/exception/diagnostic_information.hpp> // diagnostic_information
#include <boost/make_shared.hpp> // make_shared
#include <boost/shared_ptr.hpp> // shared_ptr

#include <iostream> // cout, cin, cerr
#include <string>

#include <Windows.h>
#include <tchar.h>

using winapi::gui::load_icon;
using winapi::gui::hicon;
using winapi::module_handle;

using boost::diagnostic_information;
using boost::make_shared;
using boost::shared_ptr;

using std::cerr;
using std::cout;
using std::endl;
using std::exception;
using std::string;

namespace crabgrab {

namespace {

/**
 * Return Crabgrab's tray icon.
 *
 * There is only one instance of the icon as a static variable.  This ensures
 * we don't clutter the tray with multiple, possibly stale icons.
 */
notification_icon& tray_icon()
{
    static notification_icon icon(
        load_icon(module_handle("user32.dll"), 104, 16, 16).get());
    return icon;
}

/**
 * Display a balloon message on Crabgrab's notification icon.
 */
void notification_message(const string& title, const string& message)
{
    try
    {
        tray_icon().show_message(title, message);
    }
    catch (const exception& e)
    {
        cerr << "NOTIFICATION FAILURE:" << endl;
        cerr << diagnostic_information(e) << endl;
    }
}

void grab_window(HWND hwnd)
{
    std::vector<unsigned char> bmp = take_screenshot(hwnd);

    std::cout << "TwitPic username: ";
    std::string username;
    std::cin >> username;

    std::cout << "Password (will be shown here!): ";
    std::string password;
    std::cin >> password;

    notification_message(
        "Crabgrab", "Uploading your screenshot to TwitPic ...");

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
    catch (const exception& e)
    {
        cerr << "CLIPBOARD FAILURE: " << endl;
        cerr << diagnostic_information(e) << endl; 
        cout <<
            "Crabgrab couldn't put the link to your screenshot onto the "
            "clipboard so here it is instead: " << url << endl;
        return;
    }

    notification_message(
        "Crabgrab", "The link to your screenshot is on the clipboard.");
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
                    catch (const exception& e)
                    {
                        cerr << "Unhandled exception in keyboard hook:" << endl;
                        cerr << diagnostic_information(e) << endl;
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
}

int _tmain(int argc, _TCHAR* argv[])
{
    try
    {
        crabgrab::run();
    }
    catch (const exception& e)
    {
        cerr << "Unhandled exception:" << endl;
        cerr << diagnostic_information(e) << endl;
    }

    return 0;
}

