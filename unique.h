#pragma once

#include "compressed_pair.h"
#include <cstddef>
#include <type_traits>

template <typename T>
struct DefaultDeleter {

    DefaultDeleter() = default;

    template <typename U>
    DefaultDeleter(DefaultDeleter<U>&&) {
    }

    template <typename U>
    DefaultDeleter& operator=(DefaultDeleter<U>&&) {
        return *this;
    }

    void operator()(T* ptr) {
        delete ptr;
    }
};

template <typename T>
struct DefaultDeleter<T[]> {
    void operator()(T* ptr) {
        delete[] ptr;
    }
};

template <typename T, typename Deleter = DefaultDeleter<T>>
class UniquePtr {
public:

    UniquePtr() {
        data_.GetFirst() = nullptr;
    }

    explicit UniquePtr(std::nullptr_t) {
        data_.GetFirst() = nullptr;
    }

    explicit UniquePtr(T* ptr) {
        data_.GetFirst() = ptr;
    }

    template <typename D>
    UniquePtr(T* ptr, D&& deleter) {
        data_.GetFirst() = ptr;
        data_.GetSecond() = std::forward<D>(deleter);
    }

    UniquePtr(UniquePtr&& other) noexcept {
        data_.GetFirst() = other.Release();
        data_.GetSecond() = std::move(other.data_.GetSecond());
    }

    template <typename U, typename D>
    UniquePtr(UniquePtr<U, D>&& other) noexcept {
        data_.GetFirst() = other.Release();
        data_.GetSecond() = std::move(other.GetDeleter());
    }

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (Get() != other.Get()) {
            (data_.GetSecond())(data_.GetFirst());
            data_.GetFirst() = other.Release();
            data_.GetSecond() = std::move(other.data_.GetSecond());
        }
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) {
        Reset();
        return *this;
    }

    ~UniquePtr() {
        if (data_.GetFirst() != nullptr) {
            (data_.GetSecond())(data_.GetFirst());
        }
    }

    T* Release() {
        auto ptr = data_.GetFirst();
        data_.GetFirst() = nullptr;
        return ptr;
    }

    void Reset(T* ptr = nullptr) {
        T* old = data_.GetFirst();
        data_.GetFirst() = ptr;
        (data_.GetSecond())(old);
    }

    void Swap(UniquePtr& other) {
        std::swap(data_.GetFirst(), other.data_.GetFirst());
        std::swap(data_.GetSecond(), other.data_.GetSecond());
    }

    T* Get() const {
        return data_.GetFirst();
    }
    Deleter& GetDeleter() {
        return data_.GetSecond();
    }
    const Deleter& GetDeleter() const {
        return data_.GetSecond();
    }
    explicit operator bool() const {
        return (data_.GetFirst() != nullptr);
    }

    T& operator*() const {
        return *data_.GetFirst();
    }

    T* operator->() const {
        return data_.GetFirst();
    }

private:
    CompressedPair<T*, Deleter> data_;
};

template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
public:

    UniquePtr() {
        data_.GetFirst() = nullptr;
    }

    explicit UniquePtr(std::nullptr_t) {
        data_.GetFirst() = nullptr;
    }

    explicit UniquePtr(T* ptr) {
        data_.GetFirst() = ptr;
    }

    template <typename D>
    UniquePtr(T* ptr, D&& deleter) {
        data_.GetFirst() = ptr;
        data_.GetSecond() = std::forward<D>(deleter);
    }

    UniquePtr(UniquePtr&& other) noexcept {
        data_.GetFirst() = other.Release();
        data_.GetSecond() = std::move(other.data_.GetSecond());
    }

    template <typename U, typename D>
    UniquePtr(UniquePtr<U, D>&& other) noexcept {
        data_.GetFirst() = other.Release();
        data_.GetSecond() = std::move(other.GetDeleter());
    }

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (Get() != other.Get()) {
            (data_.GetSecond())(data_.GetFirst());
            data_.GetFirst() = other.Release();
            data_.GetSecond() = std::move(other.data_.GetSecond());
        }
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) {
        Reset();
        return *this;
    }

    T& operator[](size_t i) const {
        return data_.GetFirst()[i];
    }

    ~UniquePtr() {
        if (data_.GetFirst() != nullptr) {
            (data_.GetSecond())(data_.GetFirst());
        }
    }

    T* Release() {
        auto ptr = data_.GetFirst();
        data_.GetFirst() = nullptr;
        return ptr;
    }

    void Reset(T* ptr = nullptr) {
        T* old = data_.GetFirst();
        data_.GetFirst() = ptr;
        (data_.GetSecond())(old);
    }

    void Swap(UniquePtr& other) {
        std::swap(data_.GetFirst(), other.data_.GetFirst());
        std::swap(data_.GetSecond(), other.data_.GetSecond());
    }

    T* Get() const {
        return data_.GetFirst();
    }
    Deleter& GetDeleter() {
        return data_.GetSecond();
    }
    const Deleter& GetDeleter() const {
        return data_.GetSecond();
    }
    explicit operator bool() const {
        return (data_.GetFirst() != nullptr);
    }

    T& operator*() const {
        return *data_.GetFirst();
    }

    T* operator->() const {
        return data_.GetFirst();
    }

private:
    CompressedPair<T*, Deleter> data_;
};

template <typename Deleter>
class UniquePtr<void, Deleter> {
public:

    UniquePtr() {
        data_.GetFirst() = nullptr;
    }

    explicit UniquePtr(std::nullptr_t) {
        data_.GetFirst() = nullptr;
    }

    explicit UniquePtr(void* ptr) {
        data_.GetFirst() = ptr;
    }

    template <typename D>
    UniquePtr(void* ptr, D&& deleter) {
        data_.GetFirst() = ptr;
        data_.GetSecond() = std::forward<D>(deleter);
    }

    UniquePtr(UniquePtr&& other) noexcept {
        data_.GetFirst() = other.Release();
        data_.GetSecond() = std::move(other.data_.GetSecond());
    }

    template <typename U, typename D>
    UniquePtr(UniquePtr<U, D>&& other) noexcept {
        data_.GetFirst() = other.Release();
        data_.GetSecond() = std::move(other.GetDeleter());
    }

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (Get() != other.Get()) {
            (data_.GetSecond())(data_.GetFirst());
            data_.GetFirst() = other.Release();
            data_.GetSecond() = std::move(other.data_.GetSecond());
        }
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) {
        Reset();
        return *this;
    }

    ~UniquePtr() {
        if (data_.GetFirst() != nullptr) {
            (data_.GetSecond())(data_.GetFirst());
        }
    }

    void* Release() {
        auto ptr = data_.GetFirst();
        data_.GetFirst() = nullptr;
        return ptr;
    }

    void Reset(void* ptr = nullptr) {
        void* old = data_.GetFirst();
        data_.GetFirst() = ptr;
        (data_.GetSecond())(old);
    }

    void Swap(UniquePtr& other) {
        std::swap(data_.GetFirst(), other.data_.GetFirst());
        std::swap(data_.GetSecond(), other.data_.GetSecond());
    }

    void* Get() const {
        return data_.GetFirst();
    }
    Deleter& GetDeleter() {
        return data_.GetSecond();
    }
    const Deleter& GetDeleter() const {
        return data_.GetSecond();
    }
    explicit operator bool() const {
        return (data_.GetFirst() != nullptr);
    }

    void* operator->() const {
        return data_.GetFirst();
    }

private:
    CompressedPair<void*, Deleter> data_;
};