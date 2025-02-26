#pragma once

#include "tsm_log.h"

#include <condition_variable>
#include <deque>
#include <iostream>
#include <mutex>
#include <string>

using std::deque;

namespace tsm {

// A thread safe event queue. Any thread can call addEvent if it has a pointer
// to the event queue. The call to nextEvent is a blocking call
template<typename Event, typename LockType>
struct EventQueueT : private deque<Event>
{
    using deque<Event>::empty;
    using deque<Event>::front;
    using deque<Event>::pop_front;
    using deque<Event>::push_back;
    using deque<Event>::push_front;
    using deque<Event>::size;

  public:
    EventQueueT() = default;
    EventQueueT(EventQueueT const&) = delete;
    EventQueueT(EventQueueT&&) = delete;
    EventQueueT operator=(EventQueueT const&) = delete;
    EventQueueT operator=(EventQueueT&&) = delete;

    ~EventQueueT() { stop(); }

    // Block until you get an event
    Event nextEvent()
    {
        std::unique_lock<LockType> lock(eventQueueMutex_);
        cvEventAvailable_.wait(
          lock, [this] { return (!this->empty() || this->interrupt_); });
        if (interrupt_) {
            return Event();
        }
        const Event e = std::move(front());
        // LOG(INFO) << "Thread:" << std::this_thread::get_id()
        //          << " Popping Event:" << e.id;
        pop_front();
        return e;
    }

    void addEvent(Event const& e)
    {
        std::lock_guard<LockType> lock(eventQueueMutex_);
        // LOG(INFO) << "Thread:" << std::this_thread::get_id()
        //          << " Adding Event:" << e.id;
        push_back(e);
        cvEventAvailable_.notify_all();
    }

    void stop()
    {
        interrupt_ = true;
        cvEventAvailable_.notify_all();
        // Log the events that are going to get dumped if the queue is not
        // empty
    }

    bool interrupted() { return interrupt_; }

    void addFront(Event const& e)
    {
        std::lock_guard<LockType> lock(eventQueueMutex_);
        push_front(e);
        cvEventAvailable_.notify_all();
    }

  private:
    LockType eventQueueMutex_;
    std::condition_variable_any cvEventAvailable_;
    bool interrupt_{};
};

template<typename Event>
using EventQueue = EventQueueT<Event, std::mutex>;

} // namespace tsm
