#pragma once

class RefCounter {
public:
	RefCounter() {
		count = 0;
	}

	int add() {
		return ++count;
	}

	int remove() {
		return --count;
	}

	int get() {
		return count;
	}

private:
	int count;
};

template <typename T> class SmartPointer {
public:
	SmartPointer(T* pointer) {
		this->pointer = pointer;
		counter = new RefCounter();
		counter->add();
	}

	SmartPointer(const SmartPointer& other) {
		pointer = other.pointer;
		counter = other.counter;
		counter->add();
	}

	~SmartPointer() {
		if (counter->remove() == 0) {
			delete pointer;
			delete counter;
		}
	}

	SmartPointer& operator=(const SmartPointer& other) {
		if (this != &other) {
			if (counter->remove() == 0) {
				delete pointer;
				delete counter;
			}

			pointer = other.pointer;
			counter = other.counter;
			counter->add();
		}

		return *this;
	}

	SmartPointer duplicate()
	{
		return SmartPointer(*this);
	}

	T& operator*() { return(*pointer); }
	T* operator->() { return(pointer); }
private:
	T* pointer;
	RefCounter* counter;
};