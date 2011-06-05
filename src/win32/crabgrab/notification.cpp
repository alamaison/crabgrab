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
#include <winapi/gui/windows/window.hpp> // hwnd_t
#include <winapi/trace.hpp> // trace

#include <boost/exception/diagnostic_information.hpp> // diagnostic_information
#include <boost/exception/errinfo_api_function.hpp> // errinfo_api_function
#include <boost/exception/info.hpp> // errinfo
#include <boost/make_shared.hpp> // make_shared
#include <boost/noncopyable.hpp> // noncopyable
#include <boost/throw_exception.hpp> // BOOST_THROW_EXCEPTION

#include <exception> // exception
#include <stdexcept> // invalid_argument

#include <ShellAPI.h> // Shell_NotifyIcon, NOTIFYICONDATA

#include <Windows.h> // CreateWindowEx

using winapi::gui::hwnd_t;
using winapi::last_error;
using winapi::trace;

using boost::diagnostic_information;
using boost::enable_error_info;
using boost::errinfo_api_function;
using boost::make_shared;
using boost::noncopyable;

using std::exception;
using std::invalid_argument;
using std::string;

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
    NOTIFYICONDATA notifyicondata(hwnd_t message_window)
    {
        NOTIFYICONDATA data = NOTIFYICONDATA();
        //data.cbSize = NOTIFYICONDATA_V2_SIZE;
        data.cbSize = sizeof(data);
        data.hWnd = message_window.get();
        return data;
    }

    DWORD message_icon_to_info_flags(message_icon::type icon_type)
    {
        switch (icon_type)
        {
        case message_icon::none:
            return NIIF_NONE;
        case message_icon::information:
            return NIIF_INFO;
        case message_icon::warning:
            return NIIF_WARNING;
        case message_icon::error:
            return NIIF_ERROR;
        default:
            BOOST_THROW_EXCEPTION(invalid_argument("Unknown icon type"));
        }
    }

    /**
     * Create an instance of the special window class meant only for recieving
     * and dispatching window messages.
     */
    hwnd_t create_message_only_window()
    {
        HWND raw_handle = ::CreateWindowEx(
            0, "Message", NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, NULL, NULL);
        if (raw_handle == NULL)
            BOOST_THROW_EXCEPTION(
                enable_error_info(last_error()) <<
                errinfo_api_function("CreateWindowEx"));

        return hwnd_t(raw_handle, ::DestroyWindow);
    }

    class notification_system : noncopyable
    {
    public:
        notification_system()
            : m_message_window(create_message_only_window()),
              m_current_max_id(0) {}

        hwnd_t message_window()
        {
            return m_message_window;
        }

        unsigned int next_window_id()
        {
            return ++m_current_max_id;
        }

    private:
        const winapi::gui::hwnd_t m_message_window;
        unsigned int m_current_max_id;
    };

    /**
     * Manage static 'notification system'.
     *
     * This is the global data shared by all notification items using our
     * notification wrapper.
     */
    notification_system& system()
    {
        static notification_system system;
        return system;
    }

    void shell_notify_icon(DWORD action, NOTIFYICONDATA& data)
    {
        if (::Shell_NotifyIcon(action, &data) == FALSE)
            BOOST_THROW_EXCEPTION(
                enable_error_info(last_error()) <<
                errinfo_api_function("Shell_NotifyIcon"));
    }

}

class notification_icon::icon_impl : private noncopyable
{
public:
    icon_impl(
        unsigned int icon_id, hwnd_t message_window, HICON icon_handle)
        : m_icon_id(icon_id), m_message_window(message_window)
    {
        NOTIFYICONDATA data = notifyicondata(m_message_window);

        data.uID = icon_id;

        data.uFlags = NIF_ICON;// | NIF_STATE | NIF_ICON;

        //data.dwState = NIS_SHAREDICON;
        //data.dwStateMask = NIS_SHAREDICON;

        //data.uVersion = NOTIFYICON_VERSION;
        //::Shell_NotifyIcon(NIM_SETVERSION, &data);

        shell_notify_icon(NIM_ADD, data);
    }

    ~icon_impl()
    {
        try
        {
            NOTIFYICONDATA data = notifyicondata(m_message_window);

            data.uID = m_icon_id;

            shell_notify_icon(NIM_DELETE, data);
        }
        catch (const exception& e)
        {
            trace("Error while destroying a notification: %s") %
                diagnostic_information(e);
        }
    }
    
    /**
     * @todo  Only use limited buffer on old versions of Windows (rather than
     *        array_length.
     */
    void show_message(
        const string& title, const string& message,
        message_icon::type icon_type)
    {
        NOTIFYICONDATA data = notifyicondata(m_message_window);

        data.uID = m_icon_id;

        data.uFlags = NIF_INFO;// | NIF_STATE | NIF_ICON;

        message.copy(data.szInfo, array_length(data.szInfo));
        title.copy(data.szInfoTitle, array_length(data.szInfoTitle));
        data.dwInfoFlags = message_icon_to_info_flags(icon_type);

        //data.uVersion = NOTIFYICON_VERSION;
        //::Shell_NotifyIcon(NIM_SETVERSION, &data);

        shell_notify_icon(NIM_MODIFY, data);
    }

private:
    unsigned int m_icon_id;
    hwnd_t m_message_window;
};

////////////////////////////////////////////////////////////////////////////////

notification_icon::notification_icon(HICON icon_handle)
    :
m_impl(
    make_shared<icon_impl>(
        system().next_window_id(), system().message_window(), icon_handle)) {}

void notification_icon::show_message(
    const string& title, const string& message, message_icon::type icon_type)
{
    m_impl->show_message(title, message, icon_type);
}

}
