/**
 * @file libevent.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-01-16
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

extern "C"
{
#include <signal.h>

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>
}

int test_libevent();