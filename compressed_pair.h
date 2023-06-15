#pragma once

#include <type_traits>
#include <memory>

template <typename T1, typename T2,
          bool is_compressed_T1 = std::is_empty_v<T1> && !std::is_final_v<T1>,
          bool is_compressed_T2 = std::is_empty_v<T2> && !std::is_final_v<T2>,
          bool is_same = std::is_same_v<T1, T2>>
class CompressedPair;

template <typename T1, typename T2>
class CompressedPair<T1, T2, 0, 0, 0> {
public:
    CompressedPair() : first_(T1()), second_(T2()) {
    }

    template <typename U1, typename U2>
    CompressedPair(U1&& first, U2&& second)
        : first_(std::forward<U1>(first)), second_(std::forward<U2>(second)) {
    }

    T1& GetFirst() {
        return first_;
    }

    const T1& GetFirst() const {
        return first_;
    }

    T2& GetSecond() {
        return second_;
    }

    const T2& GetSecond() const {
        return second_;
    };

private:
    T1 first_;
    T2 second_;
};

template <typename T1, typename T2>
class CompressedPair<T1, T2, 0, 0, 1> : private CompressedPair<T1, T2, 0, 0, 0> {};

template <typename T1, typename T2>
class CompressedPair<T1, T2, 0, 1, 0> : T2 {
public:
    CompressedPair() : first_(T1()) {
    }

    template <typename U1, typename U2>
    CompressedPair(U1&& first, U2&& second)
        : first_(std::forward<U1>(first)), T2(std::forward<U2>(second)) {
    }

    T1& GetFirst() {
        return first_;
    }

    const T1& GetFirst() const {
        return first_;
    }

    T2& GetSecond() {
        return *this;
    }

    const T2& GetSecond() const {
        return *this;
    };

private:
    T1 first_;
};

template <typename T1, typename T2>
class CompressedPair<T1, T2, 0, 1, 1> : private CompressedPair<T1, T2, 0, 1, 0> {};

template <typename T1, typename T2>
class CompressedPair<T1, T2, 1, 0, 0> : T1 {
public:
    CompressedPair() : second_(T2()) {
    }

    template <typename U1, typename U2>
    CompressedPair(U1&& first, U2&& second)
        : T1(std::forward<U1>(first)), second_(std::forward<U2>(second)) {
    }

    T1& GetFirst() {
        return *this;
    }

    const T1& GetFirst() const {
        return *this;
    }

    T2& GetSecond() {
        return second_;
    }

    const T2& GetSecond() const {
        return second_;
    };

private:
    T2 second_;
};

template <typename T1, typename T2>
class CompressedPair<T1, T2, 1, 0, 1> : private CompressedPair<T1, T2, 1, 0, 0> {};

template <typename T1, typename T2>
class CompressedPair<T1, T2, 1, 1, 0> : T1, T2 {
public:
    CompressedPair() = default;

    template <typename U1, typename U2>
    CompressedPair(U1&& first, U2&& second)
        : T1(std::forward<U1>(first)), T2(std::forward<U2>(second)) {
    }

    T1& GetFirst() {
        return *this;
    }

    const T1& GetFirst() const {
        return *this;
    }

    T2& GetSecond() {
        return *this;
    }

    const T2& GetSecond() const {
        return *this;
    };
};

template <typename T1, typename T2>
class CompressedPair<T1, T2, 1, 1, 1> : T1 {
public:
    CompressedPair() = default;

    template <typename U1, typename U2>
    CompressedPair(U1&& first, U2&& second)
        : first_(std::forward<U1>(first)), T2(std::forward<U2>(second)) {
    }

    T1& GetFirst() {
        return first_;
    }

    const T1& GetFirst() const {
        return first_;
    }

    T2& GetSecond() {
        return *this;
    }

    const T2& GetSecond() const {
        return *this;
    };

private:
    T1 first_;
};