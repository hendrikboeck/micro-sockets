# micro-sockets

A lightweight, header-only C library for simplified network socket programming.

## Features

- **Ease of use:** Provides simple functions for creating, connecting, and managing TCP and UDP sockets.
- **Cross-platform support:** Designed to work seamlessly on Windows, Linux, and macOS.
- **Header-only implementation:** No compilation or linking required; just include the necessary headers in your project.

## Getting Started

1. **Include the headers:**

```c
#include "micro-sockets/tcp.h"
#include "micro-sockets/udp.h"  // work in progress, not implemented yet
```

2. **Refer to the documentation:** (See Usage/Examples section below)

## Usage / Examples

**TCP Server**

also see [examples/tcp_server.c](./examples/tcp_server.c)

```c
#include <stdint.h>
#include <stdlib.h>

#include "ccms/_macros.h"
#include "ccms/box.h"
#include "ccms/sized_memory.h"
#include "micro-sockets/tcp.h"

int32_t main(void) {
  // Create a new TCP server listening on 0.0.0.0:4040
  TcpServer* server = tcp_server__new("0.0.0.0", 4040, 0);

  // Attach a buffer of size 4KiB to the server for receiving data
  tcp_server__attach_buf(server, sized_memory__new(KiB(4)));

  // Start listening for incoming connections, with a backlog of 5
  tcp_server__listen(server, 5);

  // Accept a new connection from a client
  TcpConnection conn = tcp_server__accept(server);

  // Receive data from the client
  Box data = tcp_server__recv(server, &conn);
  printf("[server] received: '%s'\n", _M_cast(char*, data.ptr));

  // Prepare a response message
  const char* resp = "hello from server!";

  // Send the response to the client
  tcp_connection__send(&conn, box__ctor(_M_cast(uint8_t*, resp), strlen(resp)));
  printf("[server] send: '%s'\n", resp);

  // Close the connection
  tcp_connection__close(&conn);

  // Shutdown and free the server
  tcp_server__shutdown(server);
  tcp_server__free(server);

  return EXIT_SUCCESS;
}
```

**TCP Client**

also see [examples/tcp_client.c](./examples/tcp_client.c)

```c
#include <stdint.h>
#include <stdlib.h>

#include "ccms/_macros.h"
#include "ccms/sized_memory.h"
#include "micro-sockets/tcp.h"

int32_t main(void) {
  // Create a new TCP client and connect to the server at 127.0.0.1:4040
  TcpClient* client = tcp_client__new("127.0.0.1", 4040, 0);
  tcp_client__connect(client);

  // Prepare a message to send to the server
  const char* msg = "hello from client!";

  // Send the message to the server
  tcp_client__send(client, box__ctor(_M_cast(uint8_t*, msg), strlen(msg)));
  printf("[client] send: '%s'\n", msg);

  // Attach a buffer of size 4KiB to the client for receiving data
  tcp_client__attach_buf(client, sized_memory__new(KiB(4)));

  // Receive a response from the server
  Box resp = tcp_client__recv(client);
  printf("[client] received: '%s'\n", _M_cast(char*, resp.ptr));

  // Close the connection and free the client
  tcp_client__close(client);
  tcp_client__free(client);

  return EXIT_SUCCESS;
}
```

## Building

### Dependencies

- CCMS, [https://github.com/hendrikboeck/ccms](https://github.com/hendrikboeck/ccms)

### Using Xmake (PPA)

Add the following lines to your `xmake.lua`

```lua
add_repositories("hendrikboeck-ppa https://github.com/hendrikboeck/xmake-ppa.git main")
add_requires("micro-sockets")

-- ...

target("<your-target>")
  -- ...
  add_packages("micro-sockets")
```

### Using Xmake (local repository)

```sh
# install dependencies to build folder

git clone https://github.com/hendrikboeck/micro-sockets.git
xmake package -P micro-sockets -D -o ./build
rm -rf micro-sockets
```

### Barebones

```sh
# install dependecies to include folder

git clone https://github.com/hendrikboeck/micro-sockets.git
cp -r micro-sockets/include/* <your-include-folder>/
rm -rf micro-sockets
```

## Contributing

Contributions are welcome! Feel free to open issues or submit pull requests.

## License

`micro-sockets` is licensed under the GNU General Public License v3.0 or later (GPLv3+). A copy of the license can be found here: https://www.gnu.org/licenses/gpl-3.0.en.html