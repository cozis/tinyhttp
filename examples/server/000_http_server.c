#include <stdio.h>
#include <stdbool.h>
#include <chttp.h>

// This example shows how to set up a basic HTTP server

int main(void)
{
    http_global_init();

    // Choose the interface to listen on and the port.
    // Currently, servers can only bind to IPv4 addresses.
    HTTP_String addr = HTTP_STR("127.0.0.1");
    uint16_t    port = 8080;

    bool all_interfaces = false;

    // If you want to bind to all interfaces, you can
    // set the address to an empty string.
    if (all_interfaces)
        addr = HTTP_STR("");

    // Instanciate the HTTP server object
    HTTP_Server *server = http_server_init(addr, port);
    if (server == NULL)
        return -1;

    // Now we loop forever. Every iteration will serve
    // a single HTTP request
    for (;;) {

        HTTP_Request *req;
        HTTP_ResponseBuilder builder;

        // Block until a request is available
        int ret = http_server_wait(server, &req, &builder);

        // The wait functions returns 0 on success and -1
        // on error. By "error" I mean an unrecoverable
        // condition. There is no other option than kill
        // the process.
        if (ret < 0)
            return -1;

        // The request information is accessible from
        // the [req] variable. Most fields in the request
        // struct are reference to the original request
        // string. They use type HTTP_String and are not
        // null-terminated. This means you'll have to make
        // sure to express the length when interacting with
        // libc:
        HTTP_String path = req->url.path;
        printf("requested path [%.*s]\n", HTTP_UNPACK(path));

        // To find a specific header value, you can either
        // iterate over the [req->headers] array or use
        // a helper function. Note that this compares header
        // names case-insensitively.
        int idx = http_find_header(req->headers, req->num_headers, HTTP_STR("Some-Header-Name"));
        if (idx == -1) {
            // Header wasn't found
        } else {
            // Found
            HTTP_String value = req->headers[idx].value;
            printf("Header has value [%.*s]\n", HTTP_UNPACK(value));
        }

        // To create a response, you will need to specify
        // status code, headers, and content in the proper
        // order.

        // First the status code
        http_response_builder_status(builder, 200);

        // Then zero or more headers
        http_response_builder_header(builder, HTTP_STR("Content-Type: text/plain"));

        // Then you can write zero or more chunks of the response body
        http_response_builder_body(builder, HTTP_STR("Hello"));
        http_response_builder_body(builder, HTTP_STR(", world!"));

        // Then, mark the request as complete (Very important or the server will hang!)
        http_response_builder_done(builder);

        // Note that none of the http_response_builder_* functions return errors.
        // This is by design to simplify user endpoint code. If at any point
        // something goes wrong, the server will send a code 4xx or 5xx to
        // the client or abort the TCP connection entirely.
    }

    // This program will loop forever, but if you write
    // your server in a way to exit gracefully, this is
    // you the server object is freed:
    http_server_free(server);
    http_global_free();

    // Have fun. Bye!
    return 0;
}