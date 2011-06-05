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

#ifndef CRABGRAB_WIN32_CRABGRAB_HOOK_HPP
#define CRABGRAB_WIN32_CRABGRAB_HOOK_HPP

#include <winapi/hook.hpp> // hhook

#include <boost/function.hpp> // function

/**
 * @file
 *
 * The Windows implementation of the keyboard hook has to use a separate
 * .cpp file as it needs to store global data that must be set and can't
 * be Constructed On First Use.
 */

namespace crabgrab {

typedef winapi::hhook keyboard_hook;

/**
 * Installs Crabgrab's keyboard hook which triggers the given screenshotting
 * action when the Print Screen key is pressed.
 *
 * The parameter to the action indicates whether it should take a screenshot of
 * the whole screen or just the current windows.  @c true indicates that the
 * screenshot should include the whole screen.
 *
 * The object returned controls the lifetime of the hook.  The caller must
 * maintain a copy of it for as long as they wish the hook to remain alive
 * @note  Not all implementations will return a real object, for instance if 
 *        they don't support uninstalling the keyboard hook.  Nevertheless,
 *        callers should treat the return value as a copyable object for
 *        portability.
 *
 * The Windows implementation executes the action in a new thread to prevent
 * Windows thinking our hook is unresponsive and uninstalling it.
 */
keyboard_hook install_keyboard_hook(
    const boost::function<void (bool)>& screenshot_action);

/**
 * Run whatever event loop is needed for the keyboard hook.
 */
void listen();

}

#endif
