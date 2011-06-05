/**
    @file

    Main crabgrab loop.

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

#include "crabgrab/clipboard.hpp" // put_clipboard_text
#include "crabgrab/encode_bmp.hpp"
#include "crabgrab/feedback.hpp" // information_message, error_message
#include "crabgrab/keyboard_hook.hpp" // install_keyboard_hook
#include "crabgrab/screenshot.hpp" // take_screenshot
#include "crabgrab/twitpic/response.hpp" // handle_response
#include "crabgrab/twitpic/twitpic.hpp"

#include <boost/exception/diagnostic_information.hpp> // diagnostic_information

#include <iostream> // cout, cin, cerr
#include <string>

#include <Windows.h>
#include <tchar.h>

using boost::diagnostic_information;

using std::cerr;
using std::cout;
using std::endl;
using std::exception;
using std::string;

namespace crabgrab {
    
namespace {

void grab_window(bool use_entire_window)
{
    std::vector<unsigned char> bmp = take_screenshot(use_entire_window);

    std::cout << "TwitPic username: ";
    std::string username;
    std::cin >> username;

    std::cout << "Password (will be shown here!): ";
    std::string password;
    std::cin >> password;

    information_message(
        "Crabgrab", "Uploading your screenshot to TwitPic ...");

    std::string xml_response = twitpic::upload_image(
        username, password, encode_as_png(bmp));

    std::string url;
    try
    {
        url = twitpic::handle_response(xml_response);
    }
    catch (const twitpic::twitpic_exception& e)
    {
        error_message("Upload failed", e.what());
        return;
    }

    try
    {
        put_clipboard_text(url);
    }
    catch (const exception& e)
    {
        error_message("Crabgrab error", "Unable to save URL to the clipboard");
        cerr << "CLIPBOARD FAILURE: " << endl;
        cerr << diagnostic_information(e) << endl;
        cout <<
            "Crabgrab couldn't put the link to your screenshot onto the "
            "clipboard so here it is instead: " << url << endl;
        return;
    }

    information_message(
        "Crabgrab", "The link to your screenshot is on the clipboard.");
}

void run()
{
    keyboard_hook hook = install_keyboard_hook(grab_window);

    MSG message;
    while (::GetMessage(&message, NULL, 0, 0) != 0)
    {
        ::TranslateMessage(&message);
        ::DispatchMessage(&message);
    }
}

}
}

int _tmain(int argc, _TCHAR* argv[])
{
    try
    {
        crabgrab::run();
    }
    catch (const exception& e)
    {
        cerr << "Unhandled exception:" << endl;
        cerr << diagnostic_information(e) << endl;
    }

    return 0;
}

