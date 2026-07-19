#pragma once
#pragma once

#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include <functional>
#include <unordered_map>
#include <typeindex>
#include <vector>
#include <memory>
#include <algorithm>

#include "Listener.h"

// Subscription handle
struct Subscription {
    std::type_index type;
    size_t id;
};

class EventManager {
    struct ErasedList {
        size_t nextId = 1;
        std::unordered_map<size_t, Listener*> callbacks;
    };
    std::unordered_map<std::type_index, ErasedList> map;

public:
    template<typename T>
    Subscription subscribe(Listener* l) {
        auto& list = map[std::type_index(typeid(T))];
        size_t id = list.nextId++;
        list.callbacks.emplace(id, l);
        return Subscription{ std::type_index(typeid(T)), id };
    }

    void unsubscribe(const Subscription& s) {
        auto it = map.find(s.type);
        if (it == map.end()) return;
        it->second.callbacks.erase(s.id);
    }

    template<typename T>
    void trigger(const T& ev) {
        auto it = map.find(std::type_index(typeid(T)));
        if (it == map.end()) return;
        // copy to allow modifications during iteration
        auto snapshot = it->second.callbacks;
        for (auto& p : snapshot) p.second->call(&ev);
    }
};

#endif