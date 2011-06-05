/**
    @file

    User notification.

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

#ifndef CRABGRAB_WIN32_CRABGRAB_NOTIFICATION_HPP
#define CRABGRAB_WIN32_CRABGRAB_NOTIFICATION_HPP

#include <boost/shared_ptr.hpp> // shared_ptr

#include <string>

#include <Windows.h> // HICON

namespace crabgrab {

namespace message_icon {
    enum type
    {
        none,
        information,
        warning,
        error,
        default = none
    };
}

class notification_icon
{
public:
    void show_message(
        const std::string& title, const std::string& message,
        message_icon::type icon_type=message_icon::default);

    notification_icon(HICON icon);

private:
    // No matter how many times this icon is copied, it must only be deleted
    // from the system tray once.  Therefore all icon instances share a single
    // implementation instance.  Essentially, this makes notification_icon an
    // augmented shared_ptr interface.
    class icon_impl;
    boost::shared_ptr<icon_impl> m_impl;
};

}

#endif
