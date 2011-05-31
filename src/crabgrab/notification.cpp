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

#include "crabgrab/notification.hpp"

#include <winapi/error.hpp> // last_error
#include <winapi/gui/icon.hpp> // load_standard_icon
#include <winapi/trace.hpp> // trace

#include <boost/exception/diagnostic_information.hpp> // diagnostic_information
#include <boost/exception/errinfo_api_function.hpp> // errinfo_api_function
#include <boost/exception/info.hpp> // errinfo
#include <boost/throw_exception.hpp> // BOOST_THROW_EXCEPTION

#include <exception> // exception

#include <ShellAPI.h>

using winapi::gui::hwnd_t;

namespace crabgrab {

namespace {
    template<typename T, size_t N>
    inline size_t array_length(const T(&)[N])
    {
        return N;
    }

    /**
     * @todo Set size by sniffing OS version.
     */
    NOTIFYICONDATA notifyicondata()
    {
        NOTIFYICONDATA data = NOTIFYICONDATA();
        //data.cbSize = NOTIFYICONDATA_V2_SIZE;
        data.cbSize = sizeof(data);
        return data;
    }

    hwnd_t create_message_only_window()
    {
        HWND raw_handle = ::CreateWindowEx(
            0, "Message", NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, NULL, NULL);
        if (raw_handle == NULL)
            BOOST_THROW_EXCEPTION(
                boost::enable_error_info(winapi::last_error()) <<
                boost::errinfo_api_function("CreateWindowEx"));

        return hwnd_t(raw_handle, ::DestroyWindow);
    }

    void shell_notify_icon(DWORD action, NOTIFYICONDATA& data)
    {
        if (::Shell_NotifyIcon(action, &data) == FALSE)
            BOOST_THROW_EXCEPTION(
                boost::enable_error_info(winapi::last_error()) <<
                boost::errinfo_api_function("Shell_NotifyIcon"));
    }
}

notification_system::notification_system()
    : m_message_window(create_message_only_window()), m_current_max_id(0)
{

}

notification_system::~notification_system()
{
    for (unsigned int i = 1; i <= m_current_max_id; ++i)
    {
        try
        {
            NOTIFYICONDATA data = notifyicondata();

            data.hWnd = m_message_window.get();
            data.uID = i;

            shell_notify_icon(NIM_DELETE, data);
        }
        catch (const std::exception& e)
        {
            winapi::trace("Error while destroying a notification: %s") %
                boost::diagnostic_information(e);
        }
    }
}

unsigned int notification_system::add_icon(HICON icon_handle)
{
    NOTIFYICONDATA data = notifyicondata();

    data.hWnd = m_message_window.get();
    data.uID = ++m_current_max_id;

    data.uFlags = NIF_ICON;// | NIF_STATE | NIF_ICON;


    //data.dwState = NIS_SHAREDICON;
    //data.dwStateMask = NIS_SHAREDICON;

    //data.hIcon = winapi::gui::load_standard_icon(
    //    winapi::gui::standard_icon_type::application);

    //data.uVersion = NOTIFYICON_VERSION;
    //::Shell_NotifyIcon(NIM_SETVERSION, &data);

    shell_notify_icon(NIM_ADD, data);

    return data.uID;
}

void notification_system::notification_message(
    unsigned int icon_id,
    const std::string& title, const std::string& message)
{
    NOTIFYICONDATA data = notifyicondata();

    data.hWnd = m_message_window.get();
    data.uID = icon_id;

    data.uFlags = NIF_INFO;// | NIF_STATE | NIF_ICON;

    message.copy(data.szInfo, array_length(data.szInfo));
    title.copy(data.szInfoTitle, array_length(data.szInfoTitle));
    data.dwInfoFlags = NIIF_INFO;// | NIIF_LARGE_ICON;

    //data.dwState = NIS_SHAREDICON;
    //data.dwStateMask = NIS_SHAREDICON;

    //data.hIcon = winapi::gui::load_standard_icon(
    //    winapi::gui::standard_icon_type::application);

    //data.uVersion = NOTIFYICON_VERSION;
    //::Shell_NotifyIcon(NIM_SETVERSION, &data);

    shell_notify_icon(NIM_MODIFY, data);
}

}
