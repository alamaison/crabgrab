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

#include "crabgrab/twitpic/response.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/throw_exception.hpp> // BOOST_THROW_EXCEPTION

#include <sstream> // istringstream

namespace crabgrab {
namespace twitpic {

std::string handle_response(const std::string& xml_response)
{
    boost::property_tree::ptree pt;

    read_xml(std::istringstream(xml_response), pt);

    std::string status = pt.get<std::string>("rsp.<xmlattr>.stat");
    if (status == "fail")
        BOOST_THROW_EXCEPTION(
            twitpic_exception(
                pt.get<int>("rsp.err.<xmlattr>.code"),
                pt.get<std::string>("rsp.err.<xmlattr>.msg")));

    // Sometime TwitPic returns what looks like a success response even though
    // it failed.  This seems to be when we have uploaded and invalid file.
    // In this strange case the media ID is the string "0".
    if (pt.get<std::string>("rsp.mediaid") == "0")
        BOOST_THROW_EXCEPTION(twitpic_exception(0, "Upload failed"));

    return pt.get<std::string>("rsp.mediaurl");
}

}}
