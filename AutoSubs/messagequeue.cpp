#include "messagequeue.h"

#include <pluglog.h>

namespace jami {

MessageQueue::MessageQueue()
    : m_event_listeners{}
    , event_loop_thread{[this] {
        while (running) {
            std::string msg;
            {
                std::unique_lock lock(m_event_mtx);
                cv.wait(lock, [this] { return !running || updated; });
                if (!running) {
                    break;
                }
                msg = m_msg;
                updated = false;
            }
            {
                std::lock_guard guard(m_event_listeners_mtx);
                for (auto& listener : m_event_listeners) {
                    listener(m_msg);
                }
            }
        }
    }}
{
}

MessageQueue::~MessageQueue()
{
    running = false;
    cv.notify_all();
    if (event_loop_thread.joinable()) {
        event_loop_thread.join();
    }
}

void MessageQueue::setMessage(const std::string& msg)
{
    std::lock_guard guard(m_mtx);
    m_msg = msg;
    updated = true;
    cv.notify_all();
}

std::string MessageQueue::message() const
{
    std::lock_guard guard(m_mtx);
    return m_msg;
}

void MessageQueue::addEventListener(MessageQueue::event_listener_t&& listener)
{
    std::lock_guard lock(m_event_listeners_mtx);
    m_event_listeners.push_back(std::move(listener));
}

void MessageQueue::removeAllEventListeners()
{
    std::lock_guard lock(m_event_listeners_mtx);
    m_event_listeners.clear();
}
}
