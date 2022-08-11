#pragma once

#include <new>
#include <utility>

namespace restrict
{

namespace detail
{

template<typename Impl, typename Base>
constexpr Impl& crtp(Base* base) noexcept
{
    return *static_cast<Impl*>(base);
}

template<typename Impl, typename Base>
constexpr const Impl& crtp(const Base* base) noexcept
{
    return *static_cast<const Impl*>(base);
}

template<typename T>
constexpr T* launder__(T* p) noexcept
{
#if __cplusplus >= 201703L
        return std::launder(p);
#else
        return p;
#endif
}

template<
    typename Impl, typename T, 
    bool = std::is_move_constructible<T>::value,
    bool = std::is_copy_constructible<T>::value>
struct raw_storage_ctor_impl
{
    static_assert(
        std::is_move_constructible<T>::value ||
        std::is_copy_constructible<T>::value, 
        "T must be copy or move constructible");
};

template<typename Impl, typename T, bool copy_construct>
struct raw_storage_ctor_impl<Impl, T, true, copy_construct>
{
    raw_storage_ctor_impl(T& t)
        noexcept(std::is_nothrow_move_constructible<T>::value)
    {
        new(crtp<Impl>(this).data()) T(std::move(t));
    }
};

template<typename Impl, typename T>
struct raw_storage_ctor_impl<Impl, T, false, true>
{
    raw_storage_ctor_impl(const T& t)
        noexcept(std::is_nothrow_copy_constructible<T>::value)
    {
        new(crtp<Impl>(this).data()) T(t);
    }
};

template<
    typename Impl, typename T,
    bool = std::is_move_assignable<T>::value,
    bool = std::is_copy_assignable<T>::value>
struct raw_storage_move_to_impl
{
    static_assert(
        std::is_move_assignable<T>::value ||
        std::is_copy_assignable<T>::value,
        "T must be move or copy assignable");
};

template<typename Impl, typename T, bool copy_assign>
struct raw_storage_move_to_impl<Impl, T, true, copy_assign>
{
    void move_to(T& dest) 
        noexcept(std::is_nothrow_move_assignable<T>::value)
    {
        dest = std::move(crtp<Impl>(this).get());
    }
};

template<typename Impl, typename T>
struct raw_storage_move_to_impl<Impl, T, false, true>
{
    void move_to(T& dest) const
        noexcept(std::is_nothrow_copy_assignable<T>::value)
    {
        dest = crtp<Impl>(this).get();
    }
};

template<typename T>
class raw_storage : 
    public raw_storage_ctor_impl<raw_storage<T>, T>,
    public raw_storage_move_to_impl<raw_storage<T>, T>
{
    alignas(T) unsigned char buf[sizeof(T)];

public:
    using raw_storage_ctor_impl<raw_storage<T>, T>::raw_storage_ctor_impl;
    using raw_storage_move_to_impl<raw_storage<T>, T>::move_to;

    decltype(buf)& data() noexcept { return buf; }

    const decltype(buf)& data() const noexcept { return buf; }

    T& get() noexcept
    {
        return *launder__(reinterpret_cast<T*>(buf));
    }

    const T& get() const noexcept
    {
        return *launder__(reinterpret_cast<const T*>(buf));
    }
};

} //namespace detail

template<typename T>
class ref
{
    detail::raw_storage<T> tmp;
    T& value;

public:
    explicit ref(T& val) 
        noexcept(noexcept(detail::raw_storage<T>(val))) : 
        tmp(val), value(val) {}

    ~ref() 
        noexcept(noexcept(tmp.move_to(value)))
    {
        tmp.move_to(value);
    }

    T& get() noexcept
    {
        return tmp.get();
    }

    const T& get() const noexcept
    {
        return tmp.get();
    }
};

} //namespace restrict
