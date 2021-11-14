#pragma once
class Entity
{
protected:
	int pos_x;
	int pos_y;
	int velocity_x;
	int velocity_y;
	int speed;
	bool active = true;
public:
	void set_pos_x(int x) {
		this->pos_x = x;
	}
	int get_pos_x() {
		return this->pos_x;
	}
	int get_pos_y() {
		return this->pos_y;
	}
	void set_pos_y(int y) {
		this->pos_y = y;
	}
	void set_velocity_x(int x) {
		this->velocity_x = x;
	}
	void set_velocity_y(int y) {
		this->velocity_y = y;
	}
	bool isActive() { return active; }
	void destroy() { active = false; }
	virtual void render() {}
	virtual void update() {}

};

class Component
{
public:
	Entity* entity;
	Component(Entity* en) {
		this->entity = en;
	}
	virtual void init() {}
	virtual void update() {};
	virtual void render() {};

	virtual ~Component() {}
};
