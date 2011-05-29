/**
    @file

    Process TwitPic upload response.

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

#ifndef CRAB_TWITPIC_TWITPIC_RESPONSE_HPP
#define CRAB_TWITPIC_TWITPIC_RESPONSE_HPP

#include <exception> // exception
#include <string>

namespace crabgrab {
namespace twitpic {

class twitpic_exception : public std::exception
{
public:
    twitpic_exception(int error_code, const std::string& message)
        : std::exception(message.c_str())
    {
        m_code = error_code;
    }

    int error_code()
    {
        return m_code;
    }

private:
    int m_code;
};

std::string handle_response(const std::string& xml_response);

}}

#endif

