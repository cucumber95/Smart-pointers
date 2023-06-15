#pragma once

#include <cstddef>
#include <utility>

#include <cassert>

class SimpleCounter {
public:
    void IncRef() {
        count_++;
    }
    void DecRef() {
        count_--;
    }
    size_t RefCount() const {
        return count_;
    }

private:
    size_t count_ = 0;
};

struct DefaultDelete {
    template <typename T>
    static void Destroy(T* object) {
        delete object;
    }
};

template <typename Derived, typename Counter, typename Deleter>
class RefCounted {
public:

    void IncRef() {
        counter_.IncRef();
    }

    RefCounted& operator=(const RefCounted& other) {
        return *this;
    }

    void DecRef() {
        counter_.DecRef();
        if (counter_.RefCount() == 0) {
            Deleter::Destroy(static_cast<Derived*>(this));
        }
    }

    size_t RefCount() const {
        return counter_.RefCount();
    }

private:
    Counter counter_;
};

template <typename Derived, typename D = DefaultDelete>
using SimpleRefCounted = RefCounted<Derived, SimpleCounter, D>;

template <typename T>
class IntrusivePtr {
    template <typename Y>
    friend class IntrusivePtr;

public:

    IntrusivePtr() : ptr_(nullptr) {
    }

    IntrusivePtr(std::nullptr_t) : ptr_(nullptr) {
    }

    IntrusivePtr(T* ptr) : ptr_(ptr) {
        if (ptr_ != nullptr) {
            ptr_->IncRef();
        }
    }

    template <typename Y>
    IntrusivePtr(const IntrusivePtr<Y>& other) : ptr_(other.ptr_) {
        if (ptr_ != nullptr) {
            ptr_->IncRef();
        }
    }

    template <typename Y>
    IntrusivePtr(IntrusivePtr<Y>&& other) : ptr_(std::move(other.ptr_)) {
        other.ptr_ = nullptr;
    }

    IntrusivePtr(const IntrusivePtr& other) : ptr_(other.ptr_) {
        if (ptr_ != nullptr) {
            ptr_->IncRef();
        }
    }

    IntrusivePtr& operator=(const IntrusivePtr& other) {
        if (ptr_ == other.ptr_) {
            return *this;
        }
        if (ptr_ != nullptr) {
            ptr_->DecRef();
        }
        ptr_ = other.ptr_;
        if (ptr_ != nullptr) {
            ptr_->IncRef();
        }
        return *this;
    }

    IntrusivePtr& operator=(IntrusivePtr&& other) {
        if (ptr_ == other.ptr_) {
            return *this;
        }
        if (ptr_ != nullptr) {
            ptr_->DecRef();
        }
        ptr_ = std::move(other.ptr_);
        other.ptr_ = nullptr;
        return *this;
    }

    ~IntrusivePtr() {
        if (ptr_ != nullptr) {
            ptr_->DecRef();
        }
    }

    void Reset() {
        if (ptr_ != nullptr) {
            ptr_->DecRef();
        }
        ptr_ = nullptr;
    }

    void Reset(T* ptr) {
        if (ptr_ != nullptr) {
            ptr_->DecRef();
        }
        ptr_ = ptr;
        if (ptr_ != nullptr) {
            ptr_->IncRef();
        }
    }

    void Swap(IntrusivePtr& other) {
        std::swap(ptr_, other.ptr_);
    }

    T* Get() const {
        return ptr_;
    }

    T& operator*() const {
        return *ptr_;
    }

    T* operator->() const {
        return ptr_;
    }

    size_t UseCount() const {
        if (ptr_ == nullptr) {
            return 0;
        }
        return ptr_->RefCount();
    }
    explicit operator bool() const {
        return ptr_ != nullptr;
    }

private:
    T* ptr_;
};

template <typename T, typename... Args>
IntrusivePtr<T> MakeIntrusive(Args&&... args) {
    return IntrusivePtr(new T(std::forward<Args>(args)...));
}
