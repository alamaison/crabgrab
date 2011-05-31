/**
    @file

    Tests for Crabgrab notification mechanism.

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

#include "crabgrab/notification.hpp" // test subject

#include <winapi/dynamic_link.hpp> // module_handle
#include <winapi/gui/icon.hpp> // load_icon

#include <boost/test/unit_test.hpp>

using crabgrab::notification_system;

using winapi::gui::load_icon;
using winapi::gui::hicon;
using winapi::module_handle;

BOOST_AUTO_TEST_SUITE(notification_tests)

namespace {
    hicon dummy_icon()
    {
        return load_icon(module_handle("user32.dll"), 104, 16, 16);
    }
}

/**
 * Show an icon.
 */
BOOST_AUTO_TEST_CASE( show_icon )
{
    notification_system n;
    
    n.add_icon(dummy_icon().get());
    ::Sleep(1000);
}

/**
 * Notify the user with a simple text message.
 */
BOOST_AUTO_TEST_CASE( show_notification )
{
    notification_system n;

    n.notification_message(
        n.add_icon(dummy_icon().get()), "Testing ...",
        "Testing Crabgrab notifications");
    ::Sleep(1000);
}

/**
 * Show three separate notifications.
 */
BOOST_AUTO_TEST_CASE( show_multiple_notifications )
{
    notification_system n;

    unsigned int id = n.add_icon(dummy_icon().get());
    n.notification_message(id, "Testing ...", "1");
    ::Sleep(300);
    n.notification_message(id, "Testing ...", "2");
    ::Sleep(300);
    n.notification_message(id, "Testing ...", "3");
    ::Sleep(300);
}

BOOST_AUTO_TEST_SUITE_END();
