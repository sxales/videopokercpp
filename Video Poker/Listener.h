#pragma once

#ifndef LISTENER_H
#define LISTENER_H

class Scene;
class Event;

class Listener {
protected:
	Scene* m_scene;
public:
	Listener(Scene* scene) : m_scene(scene) {}
	virtual ~Listener() = default;

	virtual void call(const Event* evt) = 0;
};

#endif