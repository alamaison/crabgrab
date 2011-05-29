/**
    @file

    TwitPic form upload body.

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

#include "crabgrab/twitpic/generate_body.hpp"

#include <sstream>

namespace crabgrab {
namespace twitpic {

std::string generate_body(
    const std::string& username, const std::string& password,
    const std::string& boundary, const std::vector<unsigned char>& image_bits)
{
    std::ostringstream body_data(std::ios_base::binary);
    std::string crlf = "\r\n";

    body_data << "--" + boundary << crlf;
    body_data << "Content-Disposition: form-data; name=\"username\"" << crlf;
    body_data << crlf;
    body_data << username << crlf;

    body_data << "--" + boundary << crlf;
    body_data << "Content-Disposition: form-data; name=\"password\"" << crlf;
    body_data << crlf;
    body_data << password << crlf;

    body_data << "--" + boundary << crlf;
    body_data << "Content-Disposition: form-data; name=\"media\"; "
        "filename=\"dummyfile.jpg\"" << crlf;
    body_data << "Content-Type: application/octet-stream" << crlf;
    body_data << crlf;
    body_data.write(
        reinterpret_cast<const char*>(&image_bits[0]), image_bits.size());
    body_data << crlf;

    body_data << "--" + boundary + "--" << crlf;
    body_data << crlf;

    return body_data.str();
}

}}
