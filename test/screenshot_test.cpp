/**
    @file

    Tests for screenshot grabbing.

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

#include "crabgrab/screenshot.hpp" // test subject

#include <boost/test/unit_test.hpp>

#include <vector>

#include <Windows.h> // GetForegroundWindow

using crabgrab::take_screenshot;

using std::vector;

BOOST_AUTO_TEST_SUITE(screenshot_tests)

/**
 * Take complete screengrab.
 */
BOOST_AUTO_TEST_CASE( grab_whole_desktop )
{
    vector<unsigned char> bmp = take_screenshot();
    BOOST_CHECK_GT(bmp.size(), 640U * 480U * 16U);
}

/**
 * Take screenshot of current window.
 */
BOOST_AUTO_TEST_CASE( grab_single_window )
{
    vector<unsigned char> bmp = take_screenshot(::GetForegroundWindow());
    BOOST_CHECK_GT(bmp.size(), 100U);
}

BOOST_AUTO_TEST_SUITE_END();
