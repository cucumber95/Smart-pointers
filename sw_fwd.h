#pragma once

#include <exception>
#include <cassert>

class BadWeakPtr : public std::exception {};

class ControlBlockBase {
public:
    size_t GetSCounter() const {
        return shared_counter_;
    }

    size_t GetWCounter() const {
        return weak_counter_;
    }

    void IncSCounter() {
        ++shared_counter_;
    }

    void IncWCounter() {
        ++weak_counter_;
    }

    virtual void DeleteObject() {
    }

    void DecSCounter() {
        if (shared_counter_ == 1) {
            DeleteObject();
            if (weak_counter_ == 0) {
                delete this;
                return;
            }
        }
        --shared_counter_;
    }

    void DecWCounter() {
        --weak_counter_;
        if (weak_counter_ == 0 && shared_counter_ == 0) {
            delete this;
        }
    }

    virtual ~ControlBlockBase() = default;

private:
    size_t shared_counter_ = 0;
    size_t weak_counter_ = 0;
};

template <typename T>
class ControlBlockPointer : public ControlBlockBase {
public:
    ControlBlockPointer(T* ptr) : ptr_(ptr) {
    }

    T* GetPointer() const {
        return ptr_;
    }

    void DeleteObject() override {
        delete ptr_;
    }

    ~ControlBlockPointer() override = default;

private:
    T* ptr_;
};

template <typename T>
class ControlBlockHolder : public ControlBlockBase {
public:
    template <typename... Args>
    ControlBlockHolder(Args&&... args) {
        new (&storage_) T(std::forward<Args>(args)...);
    }

    T* GetPointer() {
        return reinterpret_cast<T*>(&storage_);
    }

    void DeleteObject() override {
        GetPointer()->~T();
    }

    ~ControlBlockHolder() override = default;

private:
    std::aligned_storage_t<sizeof(T), alignof(T)> storage_;
};

template <typename T>
class SharedPtr;

template <typename T>
class WeakPtr;