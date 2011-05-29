Crabgrab: social screenshotting
========================================================

Building Crabgrab
-----------------

Set up the dependencies listed below.  Then use [CMake] to configure
the project for your build environment.  This should probably be some
version of Visual Studio (Visual C++) as Crabgrab is untested with any
other compilers.

[CMake]: http://www.cmake.org/

Dependencies
------------

Building Crabgrab depends on some third-party libraries.

  * [Boost] is fast becoming essential to any modern C++ development
    and Crabgrab uses it heavily.  Any recent version should do. CMake
    should automatically detect Boost if you installed it in the usual
    ways and will set up the include and library paths as needed.

  * [winapi] is our C++ wrapper around the Windows API.  Run `git
    submodule update --init` to clone the source files into the right
    place. No seperate build process nor library path is needed
    because winapi is a header-only library.

  * [Urdl] a modern, lightweight, C++ library for accessing the web.
    Add `src/urdl/include` to the include path or put Urld into a
    directory called `urdl` in the top-level directory.  No seperate
    build process nor library path is needed as we are using Urdl in
    header-only mode.

  * [OpenSSL] is needed for HTTPS. CMake should automatically detect
    OpenSSL if you installed it in the usual ways and will set up the
    include and library paths as needed.  When running `crabgrab.exe`
    you will need to make sure the OpenSSL DLLs are available in the
    same directory or on the path.

[winapi]:  http://github.com/alamaison/winapi/
[Boost]:   http://www.boost.org/
[Urdl]:    http://think-async.com/Urdl/
[OpenSSL]: http://www.openssl.org/

Usage
-----

Make sure the OpenSSL DLLs (`libeay32.dll`, `libssl32.dll` and
`ssleay32.dll`) are on the path or in the crabgrab directory.  Run
`crabgrab.exe`, then press the Print Screen key (`PrtScn`).  Crabgrab
will prompt you for your TwitPic username and password.  Enter these
and it will upload a screenshot to TwitPic.  Once it has finished
uploading, it will show you the url to the page.

Pressing `Shift+PrtScn` will do the same but for the current window
rather than the whole screen.

Licensing
---------

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

If you modify this Program, or any covered work, by linking or
combining it with the OpenSSL project's OpenSSL library (or a modified
version of that library), containing parts covered by the terms of the
OpenSSL or SSLeay licenses, the licensors of this Program grant you
additional permission to convey the resulting work.

### Why have an exception for OpenSSL?

Crabgrab needs the [OpenSSL] library to connect via HTTPS but the
OpenSSL license is incompatible with the GPL license because it
contains an advertising clause.  To get around this, we add an
exceptiopn - a common technique used by other projects such as [wget].

If [GnuTLS] improves to the point where OpenSSL is no longer
necessary, we may remove this exception.

[wget]:    http://www.gnu.org/software/wget/
[GnuTLS]:  http://www.gnu.org/software/gnutls/
