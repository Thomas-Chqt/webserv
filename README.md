Webserv
=======

Webserv is a lightweight HTTP server built in C++98, capable of serving static websites, handling CGI execution, and supporting file uploads. It is the first C++ project in the 42 cursus.

To get more information about the subject, please refer to [Subject.pdf](Subject.pdf).

Key Features
------------

- HTTP 1.1 compliant
- Static website serving
- CGI execution support with multiple scripting or programming languages (e.g., Python, PHP)
- File upload support using PUT or POST (via HTML forms and built-in CGI)
- Configurable via a simple config file
- Supports GET, POST, PUT, and DELETE methods
- Support for session cookies
- Auto index shows the directory content when activated in the config file

Build
-----

Webserv can be built on macOS and Linux using CMake. The following commands build the server:

```sh
mkdir build
cmake -S . -B build
cmake --build build
```

To build the unit tests, use the `WEBSERV_BUILD_TESTS` CMake option.

Usage
-----
Once built, the server can be run with a configuration file passed as an argument. If no argument is provided, it defaults to configs/webserv.conf.

```sh
./build/webserv configs/webserv.conf
```

Commands:

- `reload`: Reload the configuration file without restarting the server.
- `quit`: Shut down the server.

The configuration file format is documented through comments in the default webserv.conf file. Users should also ensure that the path to the Python interpreter is valid when using CGI functionality.

Technical Details
-----------------
Webserv uses subclasses of the `IOTask` class to handle different types of I/O operations.  
Each task is executed by the `IOManager`, which ensures that all I/O operations pass through `select`.  
Shared pointers are used to manage socket and file descriptor lifetimes, ensuring that they are properly closed once all operations are completed.

Challenges
----------
Complying with the HTTP/1.1 standard, especially persistent connections and CGI protocol integration, required significant research and testing. Additionally, the use of C++98 introduced limitations, particularly with modern development practices.

Group Project and Collaboration
-------------------------------
Webserv was developed as a group project by three people. My primary focus was on request handling, which helped deepen my understanding of the HTTP protocol and socket API. This project also allowed me to enhance my Git and GitHub skills through group collaboration.