#pragma once

#include "sw_fwd.h"

#include <cstddef>
#include <cassert>
#include <memory>
#include <type_traits>

class EnableSharedFromThisBase {};

template <typename T>
class EnableSharedFromThis : public EnableSharedFromThisBase {
public:
    SharedPtr<T> SharedFromThis() {
        return SharedPtr(weak_this_);
    }

    SharedPtr<const T> SharedFromThis() const {
        return SharedPtr(const_weak_this_);
    }

    WeakPtr<T> WeakFromThis() noexcept {
        return weak_this_;
    }

    WeakPtr<const T> WeakFromThis() const noexcept {
        return const_weak_this_;
    }

private:
    WeakPtr<T> weak_this_;
    WeakPtr<const T> const_weak_this_;

    template <typename Y>
    friend class SharedPtr;
};

template <typename T>
class SharedPtr {
public:

    SharedPtr() : ptr_(nullptr), block_(nullptr) {
    }

    SharedPtr(std::nullptr_t) : ptr_(nullptr), block_(nullptr) {
    }

    template <typename Y>
    explicit SharedPtr(Y* ptr) : ptr_(ptr), block_(new ControlBlockPointer<Y>(ptr)) {
        if (block_ != nullptr) {
            block_->IncSCounter();
        }
        if constexpr (std::is_convertible_v<Y*, EnableSharedFromThisBase*>) {
            ptr_->weak_this_ = WeakPtr<T>(*this);
            ptr_->const_weak_this_ = WeakPtr<const T>(*this);
        }
    }

    SharedPtr(ControlBlockHolder<T>* block) : ptr_(block->GetPointer()), block_(block) {
        block_->IncSCounter();
        if constexpr (std::is_convertible_v<T*, EnableSharedFromThisBase*>) {
            ptr_->weak_this_ = WeakPtr<T>(*this);
            ptr_->const_weak_this_ = WeakPtr<const T>(*this);
        }
    }

    SharedPtr(const SharedPtr& other) {
        ptr_ = other.ptr_;
        block_ = other.block_;
        if (block_ != nullptr) {
            block_->IncSCounter();
        }
    }

    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other) {
        ptr_ = other.ptr_;
        block_ = other.block_;
        if (block_ != nullptr) {
            block_->IncSCounter();
        }
    }

    template <typename Y>
    SharedPtr(SharedPtr<Y>&& other) {
        ptr_ = std::move(other.ptr_);
        block_ = std::move(other.block_);
        other.ptr_ = nullptr;
        other.block_ = nullptr;
    }

    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other, T* ptr) : ptr_(ptr), block_(other.block_) {
        block_->IncSCounter();
    }

    explicit SharedPtr(const WeakPtr<T>& other) {
        if (other.UseCount() == 0) {
            throw BadWeakPtr();
        }
        ptr_ = other.ptr_;
        block_ = other.block_;
        if (block_ != nullptr) {
            block_->IncSCounter();
        }
    }

    SharedPtr& operator=(const SharedPtr& other) {
        if (block_ != nullptr) {
            block_->DecSCounter();
        }
        ptr_ = other.ptr_;
        block_ = other.block_;
        if (block_ != nullptr) {
            block_->IncSCounter();
        }
        return *this;
    }

    template <typename Y>
    SharedPtr& operator=(const SharedPtr<Y>& other) {
        if (block_ != nullptr) {
            block_->DecSCounter();
        }
        ptr_ = other.ptr_;
        block_ = other.block_;
        if (block_ != nullptr) {
            block_->IncSCounter();
        }
        return *this;
    }

    template <typename Y>
    SharedPtr& operator=(SharedPtr<Y>&& other) {
        if (block_ != nullptr) {
            block_->DecSCounter();
        }
        ptr_ = std::move(other.ptr_);
        block_ = std::move(other.block_);
        other.ptr_ = nullptr;
        other.block_ = nullptr;
        return *this;
    }

    ~SharedPtr() {
        if (block_ == nullptr) {
            return;
        }
        block_->DecSCounter();
    }

    void Reset() {
        if (block_ == nullptr) {
            return;
        }
        block_->DecSCounter();
        ptr_ = nullptr;
        block_ = nullptr;
    }

    template <typename Y>
    void Reset(Y* ptr) {
        if (block_ != nullptr) {
            block_->DecSCounter();
        }
        ptr_ = ptr;
        block_ = new ControlBlockPointer<Y>(ptr);
        if (block_ != nullptr) {
            block_->IncSCounter();
        }
    }

    void Swap(SharedPtr& other) {
        std::swap(ptr_, other.ptr_);
        std::swap(block_, other.block_);
    }

    T* Get() const {
        if (block_ != nullptr && block_->GetSCounter() == 0) {
            return nullptr;
        }
        return ptr_;
    }

    T& operator*() const {
        return *ptr_;
    }

    T* operator->() const {
        if (block_ != nullptr && block_->GetSCounter() == 0) {
            return nullptr;
        }
        return ptr_;
    }

    size_t UseCount() const {
        if (block_ == nullptr) {
            return 0;
        }
        return block_->GetSCounter();
    }

    explicit operator bool() const {
        return ptr_ != nullptr;
    }

private:
    T* ptr_ = nullptr;
    ControlBlockBase* block_ = nullptr;

    template <typename Y>
    friend class SharedPtr;

    template <typename Y>
    friend class WeakPtr;
};

template <typename T, typename U>
inline bool operator==(const SharedPtr<T>& left, const SharedPtr<U>& right) {
    return (left.Get() == right.Get());
}

template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args);

template <typename T>
class EnableSharedFromThis {
public:
    SharedPtr<T> SharedFromThis();
    SharedPtr<const T> SharedFromThis() const;

    WeakPtr<T> WeakFromThis() noexcept;
    WeakPtr<const T> WeakFromThis() const noexcept;
};

SharedPtr<T> MakeShared(Args&&... args) {
    return SharedPtr(new ControlBlockHolder<T>(std::forward<Args>(args)...));
}
