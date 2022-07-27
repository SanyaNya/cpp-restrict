namespace detail
{

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
        new(static_cast<Impl*>(this)->data()) T(std::move(t));
    }
};

template<typename Impl, typename T>
struct raw_storage_ctor_impl<Impl, T, false, true>
{
    raw_storage_ctor_impl(const T& t)
        noexcept(std::is_nothrow_copy_constructible<T>::value)
    {
        new(static_cast<Impl*>(this)->data()) T(t);
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
        dest = std::move(static_cast<Impl*>(this)->get());
    }
};

template<typename Impl, typename T>
struct raw_storage_move_to_impl<Impl, T, false, true>
{
    void move_to(T& dest) const
        noexcept(std::is_nothrow_copy_assignable<T>::value)
    {
        dest = static_cast<const Impl*>(this)->get();
    }
};

template<
    typename Impl, typename T, 
    bool = std::is_trivially_copyable<T>::value>
struct raw_storage_impl : 
    raw_storage_ctor_impl<Impl, T>,
    raw_storage_move_to_impl<Impl, T>
{
    using raw_storage_ctor_impl<Impl, T>::raw_storage_ctor_impl;
};

template<typename Impl, typename T>
struct raw_storage_impl<Impl, T, true>
{
    raw_storage_impl(T& t) noexcept
    {
        std::memcpy(static_cast<Impl*>(this)->data(), &t, sizeof(T));
    }

    void move_to(T& dest) const noexcept
    {
        std::memcpy(&dest, &static_cast<const Impl*>(this)->get(), sizeof(T));
    }
};

template<typename T>
class raw_storage : public raw_storage_impl<raw_storage<T>, T>
{
    alignas(T) unsigned char buf[sizeof(T)];

public:
    using raw_storage_impl<raw_storage<T>, T>::raw_storage_impl;

    decltype(buf)& data() noexcept { return buf; }

    const decltype(buf)& data() const noexcept { return buf; }

    T& get() noexcept
    {
#if __cplusplus >= 201703L
        return *std::launder(reinterpret_cast<T*>(buf));
#else
        return *(reinterpret_cast<T*>(buf));
#endif
    }

    const T& get() const noexcept
    {
#if __cplusplus >= 201703L
        return *std::launder(reinterpret_cast<const T*>(buf));
#else
        return *(reinterpret_cast<const T*>(buf));
#endif
    }
};

} //namespace detail

template<typename T>
class Restrict
{
    T& value;
    detail::raw_storage<T> tmp;

public:
    explicit Restrict(T& val) 
        noexcept(noexcept(detail::raw_storage<T>(val))) : 
        value(val), tmp(val) {}

    ~Restrict() 
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
