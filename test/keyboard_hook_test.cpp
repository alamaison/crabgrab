/**
    @file

    Tests for Crabgrab keyboard hooking.

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

#include "crabgrab/keyboard_hook.hpp" // test subject

#include <boost/test/unit_test.hpp>

#include <Windows.h> // MessageBeep

using crabgrab::install_keyboard_hook;
using crabgrab::keyboard_hook;

BOOST_AUTO_TEST_SUITE(keyboard_hook_tests)

namespace {
    void beep_hook(bool use_entire_screen)
    {
        ::MessageBeep((use_entire_screen) ? MB_OK : MB_ICONQUESTION);
    }
}

/**
 * Install the hook.
 * Will be uninstalled automagically at end of test.
 */
BOOST_AUTO_TEST_CASE( hook_once )
{
    keyboard_hook h = install_keyboard_hook(beep_hook);
    BOOST_CHECK(h);
}

/**
 * Install the hook repeatedly.
 */
BOOST_AUTO_TEST_CASE( hook_multiple )
{
    install_keyboard_hook(beep_hook);
    install_keyboard_hook(beep_hook);
    install_keyboard_hook(beep_hook);
}

BOOST_AUTO_TEST_SUITE_END();
