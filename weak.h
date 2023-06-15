#pragma once

#include "sw_fwd.h"

#include <cassert>

template <typename T>
class WeakPtr {
public:

    WeakPtr() : ptr_(nullptr), block_(nullptr) {
    }

    WeakPtr(const WeakPtr& other) : ptr_(other.ptr_), block_(other.block_) {
        if (block_ != nullptr) {
            block_->IncWCounter();
        }
    }

    WeakPtr(WeakPtr&& other) {
        ptr_ = std::move(other.ptr_);
        block_ = std::move(other.block_);
        other.ptr_ = nullptr;
        other.block_ = nullptr;
    }

    WeakPtr(const SharedPtr<T>& other) : ptr_(other.ptr_), block_(other.block_) {
        if (block_ != nullptr) {
            block_->IncWCounter();
        }
    }

    WeakPtr& operator=(const WeakPtr& other) {
        if (block_ != nullptr) {
            block_->DecWCounter();
        }
        ptr_ = other.ptr_;
        block_ = other.block_;
        if (block_ != nullptr) {
            block_->IncWCounter();
        }
        return *this;
    }

    template <typename Y>
    WeakPtr& operator=(const WeakPtr<Y>& other) {
        if (block_ != nullptr) {
            block_->DecWCounter();
        }
        ptr_ = other.ptr_;
        block_ = other.block_;
        if (block_ != nullptr) {
            block_->IncWCounter();
        }
        return *this;
    }

    WeakPtr& operator=(WeakPtr&& other) {
        if (block_ != nullptr) {
            block_->DecWCounter();
        }
        ptr_ = std::move(other.ptr_);
        block_ = std::move(other.block_);
        other.ptr_ = nullptr;
        other.block_ = nullptr;
        return *this;
    }

    template <typename Y>
    WeakPtr& operator=(SharedPtr<Y>& other) {
        if (block_ != nullptr) {
            block_->DecWCounter();
        }
        ptr_ = other.ptr_;
        block_ = other.block_;
        if (block_ != nullptr) {
            block_->IncWCounter();
        }
        return *this;
    }

    ~WeakPtr() {
        if (block_ == nullptr) {
            return;
        }
        block_->DecWCounter();
    }

    void Reset() {
        if (block_ == nullptr) {
            return;
        }
        block_->DecWCounter();
        ptr_ = nullptr;
        block_ = nullptr;
    }

    void Swap(WeakPtr& other) {
        std::swap(ptr_, other.ptr_);
        std::swap(block_, other.block_);
    }

    size_t UseCount() const {
        if (block_ == nullptr) {
            return 0;
        }
        return block_->GetSCounter();
    }

    bool Expired() const {
        if (block_ == nullptr) {
            return true;
        }
        return block_->GetSCounter() == 0;
    }

    SharedPtr<T> Lock() const {
        if (UseCount() == 0) {
            return SharedPtr<T>();
        }
        return SharedPtr(*this);
    }

private:
    T* ptr_ = nullptr;
    ControlBlockBase* block_ = nullptr;

    template <typename Y>
    friend class SharedPtr;

    template <typename Y>
    friend class WeakPtr;
};
