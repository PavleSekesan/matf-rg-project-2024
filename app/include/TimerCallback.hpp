//
// Created by pavle on 10/28/25.
//

#ifndef MATF_RG_PROJECT_TIMERCALLBACK_HPP
#define MATF_RG_PROJECT_TIMERCALLBACK_HPP
#include <functional>

class TimerCallback {
    float m_timeout = 0.0;
    float time = 0.0;
    bool active = false;
    std::function<void()> m_callback;

public:
    void setTimeout(float timeout, std::function<void()> callback) {
        active = true;
        time = 0.0;
        m_timeout = timeout;
        m_callback = std::move(callback);
    }

    void tick(float dt) {
        if (!active) return;
        time += dt;
        if (time > m_timeout) {
            active = false;
            m_callback();
        }
    }
};

#endif//MATF_RG_PROJECT_TIMERCALLBACK_HPP