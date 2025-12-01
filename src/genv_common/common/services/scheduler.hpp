/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * cr_scheduler.hpp - Created on 27-11-2025
 *
 * GenV is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * GenV is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * GenV. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <cassert>
#include "common/util/hash.hpp"
#include "common/util/templates.hpp"

template <typename T, typename... A>
class Promise
{
private:
    T *obj;
    Promise (T::*resumePoint)(A... args);

public:
    Promise(T *obj_, Promise (T::*resumePoint_)(A... args))
        : obj(obj_),
          resumePoint(resumePoint_)
    {
    }

    // called by the scheduler
    Promise resume(A... args)
    {
        return (obj->*resumePoint)(args...);
    }
};

typedef bool (*EventTestSource)(void);
typedef void (*SubscriberFunction)(void *arg, int argc);

typedef enum : util::Hash
{
    EVENT_VSYNC_INT = "EventVSync"_h,
    EVENT_UART_INT = "EventUART0"_h,
    EVENT_INPUT_INT = "EventInput"_h,
} BuiltinEvents;

struct EventSubscriber
{
    const char *name;
    SubscriberFunction func;
};

class EventSource
{
    friend class CoroutineScheduler;

private:
    const util::Hash id;
    util::PointerList<EventSubscriber *, 1> list;

public:
    EventSource(util::Hash id);
    inline util::Hash getID() { return id; }
};

class PolledEventSource : public EventSource
{
private:
    EventTestSource func = nullptr;

public:
    inline PolledEventSource(util::Hash id, EventTestSource func) : EventSource(id)
    {
        assert(!func);
    }
    inline bool poll() { return func(); }
};

class InterruptEventSource : public EventSource
{
private:
    bool *test = nullptr;

public:
    inline InterruptEventSource(util::Hash id, bool *testSource) : EventSource(id) {}
    inline bool poll() { return *test; }
};

class CoroutineScheduler
{
private:
public:
    CoroutineScheduler() = default;

    void update();
};