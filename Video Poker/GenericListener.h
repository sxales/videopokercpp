#pragma once

#include <functional>

#include "Listener.h"

class Scene;
class Event;

using Callback = std::function<void(Scene*, const Event*)>;

class GenericListener : public Listener {
private:
	Callback m_callback;
public:
	GenericListener(Scene* scene, Callback callback) : Listener(scene), m_callback(callback) {}

	void call(const Event* evt) {
		m_callback(m_scene, evt);
	};
};