# cHTTP
cHTTP is an HTTP **client and server** library for C with minimal dependencies and **distributed as a single chttp.c** file.

Here is a list of features:

* HTTP 1.1 client and server
* Non-blocking
* Cross-Platform
* Minimal dependencies
* HTTPS support
* Virtual Hosts
* Single-threaded
* Ergonomic API

Note: This library is beta-quality software as some essential features are still being implemented.

## Getting Started

The library is distributed as a single amalgamated `chttp.c` file or as a static library, so you can:

1) Download `chttp.c` and `chttp.h` from the repository (no need to clone the project) and include them in your soource tree

2) or clone the project and build the static library by running:

```
make libchttp.a
```

If you used the amalgamated files, these are the flags required to build a project:

```bash
# Linux
gcc your_app.c chttp.c

# Windows
gcc your_app.c chttp.c -lws2_32
```

If you are using the static library, instead of adding `chttp.c`, you will need to add the `-lchttp` flag.

By default, the library is built without HTTPS. To enable it, add the flags `-DHTTPS_ENABLED -lssl -lcrypto`.

```bash
# Linux + HTTPS
gcc your_app.c chttp.c -DHTTPS_ENABLED -lssl -lcrypto

# Windows + HTTPS
gcc your_app.c chttp.c -lws2_32 -DHTTPS_ENABLED -lssl -lcrypto
```

## Example

To give you a feel of the library, here are some examples of programs using cHTTP. To learn more, you can look at the files in `examples/` (they are intended to be skimmed in order).

Here is a client performing a GET request:
```c
#include <stdio.h>
#include <chttp.h>

int main(void)
{
    http_global_init();

    HTTP_String url = HTTP_STR("http://example.com/index.html");

    HTTP_String headers[] = {
        HTTP_STR("User-Agent: cHTTP"),
    };

    HTTP_Response *res = http_get(url, headers, 1);

    fwrite(res->body.ptr, 1, res->body.len, stdout);

    http_response_free(res);
    http_global_free();
    return 0;
}
```

And this is a server:
```c
#include <chttp.h>

int main(void)
{
    http_global_init();
    HTTP_Server *server = http_server_init(HTTP_STR("127.0.0.1"), 8080);

    for (;;) {

        HTTP_Request *req;
        HTTP_ResponseBuilder builder;
        http_server_wait(server, &req, &builder);

        http_response_builder_status(builder, 200);
        http_response_builder_header(builder, "Content-Type: text/plain");
        http_response_builder_body(builder, HTTP_STR("Hello, world!"));
        http_response_builder_done(builder);
    }

    http_server_free(server);
    http_global_free();
    return 0;
}
```

## Platform Support
cHTTP officially supports Linux and Windows.

## HTTPS support
Currently, HTTPS is implemented using OpenSSL. If you are on Windows, you'll need to install it manually or disabling it by not defining `HTTPS_ENABLED`.

## Scalability
cHTTP is designed to reach moderate scale to allow a compact and easy to work with implementation. The non-blocking I/O is based on `poll()` which I would say works up to about 500 concurrent connections. If you have more than that, you should consider APIs like epoll, io_uring, and I/O completion ports. If you do go that route, you can still reuse the cHTTP I/O independent core (see HTTP_Engine) to handle the HTTP protocol for you, both for client and server.
