#include "acceptor.h"
#include <assert.h>

struct acceptor *acceptor_init(int port)
{
    struct acceptor *acceptor1 = (struct acceptor *)malloc(sizeof(struct acceptor));
    acceptor1->listen_port = port;
    acceptor1->listen_port = socket(AF_INET, SOCK_STREAM, 0);

    make_nonblocking(acceptor1->listen_port);

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    int on = 1;
    setsockopt(acceptor1->listen_port, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    int rt1 = bind(acceptor1->listen_port, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (rt1 < 0) {
        err_sys("bind failed");
    }

    int rt2 = listen(acceptor1->listen_port, LISTENQ);
    if (rt2 < 0) {
        err_sys("listen failed");
    }

    return acceptor1;
}
