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

using crabgrab::raw_image;
using crabgrab::take_screenshot;

BOOST_AUTO_TEST_SUITE(screenshot_tests)

/**
 * Take complete screengrab.
 */
BOOST_AUTO_TEST_CASE( grab_whole_desktop )
{
    raw_image image = take_screenshot(true);

    // 4 is the size of one raw pixel in bytes
    BOOST_CHECK_GT(image.pixel_data().size(), 640U * 480U * 4U);
    BOOST_CHECK_GT(image.width(), 640U);
    BOOST_CHECK_GT(image.height(), 480U);
}

/**
 * Take screenshot of current window.
 */
BOOST_AUTO_TEST_CASE( grab_single_window )
{
    raw_image image = take_screenshot(false);

    // 4 is the size of one raw pixel in bytes
    BOOST_CHECK_GT(image.pixel_data().size(), 100U * 4U);
    BOOST_CHECK_GT(image.width(), 10U);
    BOOST_CHECK_GT(image.height(), 10U);
}

BOOST_AUTO_TEST_SUITE_END();
