#include "libevent.hpp"

static void listener_cb(struct evconnlistener*, evutil_socket_t, struct sockaddr*, int socklen, void*);
static void signal_cb(evutil_socket_t, short, void*);
static void conn_writecb(struct bufferevent*, void*);
static void conn_eventcb(struct bufferevent*, short, void*);

int test_libevent()
{
    struct event_base* base;
    struct evconnlistener* listener;
    struct event* signal_event;

    struct sockaddr_in sin = { 0 };


    base = event_base_new();
    if (!base)
    {
        fprintf(stderr, "Could not initialize libevent\n");
        return 1;
    }

    const char* event_notification_mechanism = event_base_get_method(base);
    fprintf(stderr, "kernel event notification mechanism: %s\n", event_notification_mechanism);
    const char* signal_handler_mechanism = event_base_get_signal_method(base);
    fprintf(stderr, "signal handler mechanism: %s\n", signal_handler_mechanism);
    const char** event_notification_mechanism_arr = event_get_supported_methods();
    while (*event_notification_mechanism_arr != NULL)
    {
        fprintf(stderr, "%s\n", *event_notification_mechanism_arr);
        event_notification_mechanism_arr++;
    }

    sin.sin_family = AF_INET;
    sin.sin_port = htons(9989);

    listener = evconnlistener_new_bind(base, listener_cb, (void*)base, LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1, (struct sockaddr*)&sin, sizeof(sin));

    if (!listener)
    {
        fprintf(stderr, "Could not create a listener\n");
        return 1;
    }

    signal_event = evsignal_new(base, SIGINT, signal_cb, (void*)base);
    if (!signal_event || event_add(signal_event, NULL) < 0)
    {
        fprintf(stderr, "Could not create/add a signal event\n");
        return 1;
    }

    event_base_dispatch(base);

    evconnlistener_free(listener);
    event_free(signal_event);
    event_base_free(base);

    printf("done\n");

    return 0;
}

static void listener_cb(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr* sa, int socklen, void* user_data)
{
    struct event_base* base = (struct event_base*)user_data;
    struct bufferevent* bev;

    bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    if (!bev)
    {
        fprintf(stderr, "Error constructing bufferevent!\n");
        event_base_loopbreak(base);
        return;
    }

    bufferevent_setcb(bev, NULL, conn_writecb, conn_eventcb, NULL);
    bufferevent_enable(bev, EV_WRITE);
    bufferevent_disable(bev, EV_READ);

    const char* msg = "Hello world";
    bufferevent_write(bev, msg, strlen(msg));
}

static void conn_writecb(struct bufferevent* bev, void* user_data)
{
    struct evbuffer* output = bufferevent_get_output(bev);
    if (evbuffer_get_length(output) == 0)
    {
        printf("flushed answer\n");
        bufferevent_free(bev);
    }
}

static void conn_eventcb(struct bufferevent* bev, short events, void* user_data)
{
    if (events & BEV_EVENT_EOF)
    {
        printf("Connection closed.\n");
    }
    else if (events & BEV_EVENT_ERROR)
    {
        printf("Got an error on the connection: %s \n", strerror(errno));
    }

    bufferevent_free(bev);
}

static void signal_cb(evutil_socket_t sig, short events, void* user_data)
{
    struct event_base* base = (struct event_base*)user_data;
    struct timeval delay = { 2, 0 };

    printf("Caught an interrupt signal; exiting cleanly in two seconds\n");

    event_base_loopexit(base, &delay);
}