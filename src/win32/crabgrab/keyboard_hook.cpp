/**
    @file

    Crabgrab keyboard hook on Windows.

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

#include "crabgrab/keyboard_hook.hpp"


#include <winapi/hook.hpp> // windows_hook
#include <winapi/trace.hpp> // trace

#include <boost/exception/diagnostic_information.hpp> // diagnostic_information
#include <boost/function.hpp> // function
#include <boost/thread.hpp> // thread

#include <exception> // exception

#include <Windows.h> // CallNextHookEx, KBDLLHOOKSTRUCT

using winapi::trace;
using winapi::windows_global_hook;

using boost::diagnostic_information;
using boost::function;
using boost::thread;

using std::exception;

namespace crabgrab {

namespace {

/**
 * Hook base class.
 * Wraps basic hook functions.
 */
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

/**
 * Low-level (global) keyboard hook.
 * Filters out messages we are never meant to respond to.
 */
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

/**
 * Crabgrab keyboard hook.
 * Reacts to the print-screen button by calling to hook action callable in
 * a new thread.
 */
class crabgrab_keyboard_hook : public low_level_keyboard_hook_procedure
{
public:
    crabgrab_keyboard_hook(const function<void (bool)>& action) :
      m_is_shift_pressed(false), m_action(action) {}

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
                    try
                    {
                        thread(m_action, !m_is_shift_pressed);
                    }
                    catch (const exception& e)
                    {
                        trace("Unhandled exception in keyboard hook:\n%s\n") %
                            diagnostic_information(e);
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
    function<void (bool)> m_action;
};

function<void (bool)> g_screenshot_action;

LRESULT CALLBACK hook_proc(int code, WPARAM wparam, LPARAM lparam)
{
    static crabgrab_keyboard_hook hook(g_screenshot_action);
    return hook.hook_proc(code, wparam, lparam);
}

}

keyboard_hook install_keyboard_hook(
    const function<void (bool)>& screenshot_action)
{
    // XXX: Yuk. Yuk. Yuk.  We have to store the hook action as global data
    // because there is no other way to pass it to the hook proc.  This means
    // only the first call to install_hook will have any effect.  Subsequent
    // calls will install the same hook with the same (local static) hook 
    // action.  Shame on you Microsoft for not providing a userdata parameter
    // in the callback. (Shame on me if they did provide a way to do this and
    // I didn't find it.)
    g_screenshot_action = screenshot_action;
    return windows_global_hook(WH_KEYBOARD_LL, &hook_proc);
}

}
