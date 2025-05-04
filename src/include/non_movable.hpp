#pragma once

class nonmovable {
    public:
        nonmovable() = default;
        virtual ~nonmovable() = default;
        nonmovable(const nonmovable& other) = delete;
        nonmovable(nonmovable&& other) = delete;
        nonmovable& operator=(const nonmovable& other) = delete;
        nonmovable& operator=(nonmovable&& other) = delete;
};

class noncopyable {
    public:
        noncopyable() = default;
        virtual ~noncopyable() = default;
        noncopyable(const noncopyable& other) = delete;
        noncopyable(noncopyable&& other) = default;
        noncopyable& operator=(const noncopyable& other) = delete;
        noncopyable& operator=(noncopyable&& other) = default;
};
typedef noncopyable movable;

//!@note for derived class move fallback to copy, to prevent it implicit delete move ctor in derived class
class copyable {
    public:
        copyable() = default;
        virtual ~copyable() = default;
        copyable(const copyable& other) = default;
        copyable(copyable&& other) = delete;
        copyable& operator=(const copyable& other) = default;
        copyable& operator=(copyable&& other) = delete;
};
