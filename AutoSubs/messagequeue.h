#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

#include <condition_variable>
#include <functional>
#include <list>
#include <mutex>
#include <string>
#include <thread>

namespace jami {

class MessageQueue {
public:
    MessageQueue();
    ~MessageQueue();
    void setMessage(std::string const& msg);
    std::string message() const;

    using event_listener_t = std::function<void(std::string const&)>;
    using event_listener_id = std::list<event_listener_t>::iterator;
    void addEventListener(event_listener_t&& listener);
    void removeAllEventListeners();

private:
    bool running = true;
    bool updated = false;
    std::condition_variable cv;
    std::list<event_listener_t> m_event_listeners;
    std::thread event_loop_thread;
    mutable std::mutex m_event_mtx{};
    std::mutex m_event_listeners_mtx{};

    std::string m_msg{};
};
}

#endif // MESSAGEQUEUE_H
