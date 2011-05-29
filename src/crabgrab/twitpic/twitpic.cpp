/**
    @file

    TwitPic upload.

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

#include "crabgrab/twitpic/twitpic.hpp"

#include "crabgrab/twitpic/generate_body.hpp"

#include <urdl/http.hpp>
#include <urdl/istream.hpp>

#include <iostream>
#include <sstream>

namespace crabgrab {
namespace twitpic {

std::string upload_image(
    const std::string& username, const std::string& password,
    const std::vector<unsigned char>& image_bits)
{
    urdl::istream is;
    
    std::string boundary = "CrabgrabBoundary361654164136464264983165078168";

    is.set_option(urdl::http::request_method("POST"));

    is.set_option(
        urdl::http::request_content_type(
            "multipart/form-data; boundary=" + boundary));
    is.set_option(urdl::http::user_agent("crabgrab"));

    std::string form_body = generate_body
        (username, password, boundary, image_bits);

    is.set_option(urdl::http::request_content(form_body));

    is.open("http://twitpic.com/api/upload");
    if (!is)
    {
        std::cerr << "Unable to open URL" << std::endl;
        return "";
    }

    std::ostringstream response;
    std::string line;
    while (std::getline(is, line))
    {
        response << line << std::endl;
    }
    
    return response.str();
}

}}
