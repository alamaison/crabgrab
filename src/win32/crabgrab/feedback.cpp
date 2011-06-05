/**
    @file

    Crabgrab notifications on Windows.

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

/**
 * @file
 *
 * This file contains the Windows-dependent implementation of the Crabgrab
 * notification mechanism.
 */

#include "crabgrab/feedback.hpp"

#include "crabgrab/notification.hpp" // notification_icon

#include <winapi/dynamic_link.hpp> // module_handle
#include <winapi/gui/icon.hpp> // load_icon

#include <boost/exception/diagnostic_information.hpp> // diagnostic_information

#include <iostream> // cerr
#include <exception> // exception

using winapi::gui::load_icon;
using winapi::module_handle;

using boost::diagnostic_information;

using std::cerr;
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

}

/**
 * The Windows implementation displays information as a balloon on a system-tray
 * notification icon.  The balloon has an information icon in it.
 */
void information_message(const string& title, const string& message)
{
    try
    {
        tray_icon().show_message(title, message, message_icon::information);
    }
    catch (const exception& e)
    {
        cerr << "NOTIFICATION FAILURE:" << endl;
        cerr << diagnostic_information(e) << endl;
    }
}

/**
 * The Windows implementation displays an error as a balloon on a system-tray
 * notification icon.  The balloon has an error icon in it.
 *
 * @todo  Notifications aren't really appropriate for displaying errors as they
 *        aren't guaranteed to show up.  This should probably be changed to a
 *        message box.
 */
void error_message(const string& title, const string& message)
{
    try
    {
        tray_icon().show_message(title, message, message_icon::error);
    }
    catch (const exception& e)
    {
        cerr << "NOTIFICATION FAILURE:" << endl;
        cerr << diagnostic_information(e) << endl;
    }
}

}
