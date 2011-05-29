/**
    @file

    Tests for TwitPic response processing.

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

#include "crabgrab/twitpic/response.hpp" // test subject

#include <boost/test/unit_test.hpp>

#include <string>

BOOST_AUTO_TEST_SUITE(twitpic_response_tests)

/**
 * A twitpic failure message should throw a twitpic_exception.
 */
BOOST_AUTO_TEST_CASE( failure_response_throws )
{
    std::string failure_response =
        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<rsp stat=\"fail\">\n"
        "  <err code=\"1001\" msg=\"Invalid twitter username or password\" />\n"
        "</rsp>";
    BOOST_CHECK_THROW(
        crabgrab::twitpic::handle_response(failure_response),
        crabgrab::twitpic::twitpic_exception);
}

/**
 * A twitpic success message should return the URL.
 */
BOOST_AUTO_TEST_CASE( success_response_url )
{
    std::string success_response =
        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<rsp stat=\"ok\">\n"
        "  <mediaid>546qt1</mediaid>\n"
        "  <mediaurl>http://twitpic.com/546qt1</mediaurl>\n"
        "</rsp>";
     
    std::string url = crabgrab::twitpic::handle_response(success_response);
    BOOST_CHECK_EQUAL(url, "http://twitpic.com/546qt1");
}

/**
 * Check that we throw on the bizarre failure success message.
 */
BOOST_AUTO_TEST_CASE( bizarre_response_throws )
{
    std::string bizarre_response =
        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<rsp stat=\"ok\">\n"
        "  <mediaid>0</mediaid>\n"
        "  <mediaurl>http://twitpic.com/0</mediaurl>\n"
        "</rsp>";
    BOOST_CHECK_THROW(
        crabgrab::twitpic::handle_response(bizarre_response),
        crabgrab::twitpic::twitpic_exception);
}

BOOST_AUTO_TEST_SUITE_END();
