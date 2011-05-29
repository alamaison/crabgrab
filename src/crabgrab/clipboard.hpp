/**
    @file

    Windows clipboard access.

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

#ifndef CRABGRAB_CLIPBOARD_HPP
#define CRABGRAB_CLIPBOARD_HPP

#include <winapi/error.hpp> // last_error
#include <winapi/global_lock.hpp> // global_lock
#include <winapi/trace.hpp> // trace

#include <boost/exception/diagnostic_information.hpp> // diagnostic_information
#include <boost/exception/errinfo_api_function.hpp> // errinfo_api_function
#include <boost/exception/info.hpp> // errinfo
#include <boost/noncopyable.hpp> // noncopyable
#include <boost/throw_exception.hpp> // BOOST_THROW_EXCEPTION

#include <boost/type_traits/remove_pointer.hpp> // remove_pointer

#include <exception> // exception
#include <string>

#include <Windows.h> // GlobalAlloc, GlobalFree

namespace crabgrab {

namespace detail {

    inline void open_clipboard(HWND new_owner=NULL)
    {
        if (!::OpenClipboard(new_owner))
            BOOST_THROW_EXCEPTION(
                boost::enable_error_info(winapi::last_error()) <<
                boost::errinfo_api_function("OpenClipboard"));
    }

    inline void close_clipboard()
    {
        if (!::CloseClipboard())
            BOOST_THROW_EXCEPTION(
                boost::enable_error_info(winapi::last_error()) <<
                boost::errinfo_api_function("CloseClipboard"));
    }

    class clipboard
    {
    public:
        clipboard(HWND new_owner=NULL)
        {
            open_clipboard(new_owner);
        }
        
        ~clipboard()
        {
            try
            {
                close_clipboard();
            }
            catch (const std::exception& e)
            {
                winapi::trace("Error while closing the clipboard: %s") %
                    boost::diagnostic_information(e);
            }
        }
    };

    inline void empty_clipboard()
    {
        if (!::EmptyClipboard())
            BOOST_THROW_EXCEPTION(
                boost::enable_error_info(winapi::last_error()) <<
                boost::errinfo_api_function("EmptyClipboard"));
    }

    inline void set_clipboard_data(UINT format, HANDLE global_data)
    {
        HANDLE handle = ::SetClipboardData(format, global_data);
        if (handle == NULL)
            BOOST_THROW_EXCEPTION(
                boost::enable_error_info(winapi::last_error()) <<
                boost::errinfo_api_function("SetClipboardData"));
    }

    inline HANDLE get_clipboard_data(UINT format)
    {
        HANDLE handle = ::GetClipboardData(format);
        if (handle == NULL)
            BOOST_THROW_EXCEPTION(
                boost::enable_error_info(winapi::last_error()) <<
                boost::errinfo_api_function("GetClipboardData"));

        return handle;
    }

    class global_ptr : boost::noncopyable
    {
    public:
        global_ptr(HGLOBAL handle) : m_handle(handle) {}

        HGLOBAL get()
        {
            return m_handle;
        }

        const HGLOBAL get() const
        {
            return m_handle;
        }

        HGLOBAL release()
        {
            HGLOBAL handle = m_handle;
            m_handle = NULL;
            return handle;
        }

        ~global_ptr()
        {
            ::GlobalFree(m_handle);
        }

    private:
        HGLOBAL m_handle;
    };

    /**
     * @todo  Find some way to OR these flags together.
     */
    enum global_flags
    {
        initialise_to_zero = GMEM_ZEROINIT,
        fixed = GMEM_FIXED,
        moveable = GMEM_MOVEABLE,
    };

    inline HGLOBAL global_alloc(size_t size, global_flags flags)
    {
        HGLOBAL handle = ::GlobalAlloc(flags, size);
        if (handle == NULL)
            BOOST_THROW_EXCEPTION(
                boost::enable_error_info(winapi::last_error()) <<
                boost::errinfo_api_function("GlobalAlloc"));

        return handle;
    }
}

inline void put_clipboard_text(const std::string& text)
{
    typedef std::auto_ptr<boost::remove_pointer<HGLOBAL>::type> hglobal;

    // This is nasty as we are transferring ownership of the HGLOBAL to the
    // clipboard in SetClipboardData but must still be prepared to clean it up
    // ourselves right up until we are sure that call succeeded.
    detail::global_ptr global_text(
        detail::global_alloc(text.size() + 1, detail::moveable));

    // Copy inside a locked scope
    {
        winapi::global_lock<char> lock(global_text.get());

        text.copy(lock.get(), text.size());
        lock.get()[text.size()] = '\0'; // null-terminate
    }

    detail::clipboard clipboard;
    detail::empty_clipboard();
    detail::set_clipboard_data(CF_TEXT, global_text.get());
    
    // OK, we're sure the clipboard owns the data now.  Release our grip on it.
    global_text.release();
}

inline std::string get_clipboard_text()
{
    detail::clipboard clipboard;
    HGLOBAL handle = detail::get_clipboard_data(CF_TEXT);
    
    winapi::global_lock<char> lock(handle);
    return lock.get();
}

}

#endif
