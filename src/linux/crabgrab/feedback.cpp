/**
    @file

    Crabgrab notifications on Linux.

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

/**
 * @file
 *
 * This file contains the Linux-dependent implementation of the Crabgrab
 * notification mechanism.
 *
 * At the moment, these just output to the console.
 *
 * @todo  Use proper notification mechanism.
 */

#include "crabgrab/feedback.hpp"

#include <iostream> // cerr, cout

using std::cerr;
using std::cout;
using std::endl;
using std::string;

namespace crabgrab {

void information_message(const string& title, const string& message)
{
    cout << title << ":" << endl;
    cout << message << endl;
}

void error_message(const string& title, const string& message)
{
    cerr << title << ":" << endl;
    cerr << message << endl;
}

}
