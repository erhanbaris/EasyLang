#ifndef EASYLANG_DISPACHER_H
#define EASYLANG_DISPACHER_H

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wparentheses"

#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wfloat-equal"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

struct Void { };

class Any
{
    using id = size_t;

    template<typename T>
    struct type { static void id() { } };

    template<typename T>
    static id type_id() { return reinterpret_cast<id>(&type<T>::id); }

    //-----------------------------------------------------------------------------

    template<typename T>
    using decay = typename std::decay<T>::type;

    template<typename T>
    using none = typename std::enable_if<!std::is_same<Any, T>::value>::type;

    //-----------------------------------------------------------------------------

    struct base
    {
        virtual ~base() { }
        virtual bool is(id) const = 0;
        virtual base *copy() const = 0;
    } *p = nullptr;

    //-----------------------------------------------------------------------------

    template<typename T>
    struct data : base, std::tuple<T>
    {
        using std::tuple<T>::tuple;

        T       &get()      & { return std::get<0>(*this); }
        T const &get() const& { return std::get<0>(*this); }

        bool is(id i) const override { return i == type_id<T>(); }
        base *copy()  const override { return new data{ get() }; }
    };

    //-----------------------------------------------------------------------------

    template<typename T>
    T &stat() { return static_cast<data<T>&>(*p).get(); }

    template<typename T>
    T const &stat() const { return static_cast<data<T> const&>(*p).get(); }

    template<typename T>
    T &dyn() { return dynamic_cast<data<T>&>(*p).get(); }

    template<typename T>
    T const &dyn() const { return dynamic_cast<data<T> const&>(*p).get(); }

    //-----------------------------------------------------------------------------

public:
    Any() { }
    ~Any() { delete p; }

    Any(Any &&s) : p{ s.p } { s.p = nullptr; }
    Any(Any const &s) : p{ s.p->copy() } { }

    template<typename T, typename U = decay<T>, typename = none<U>>
    Any(T &&x) : p{ new data<U>{ std::forward<T>(x) } } { }

    Any &operator=(Any s) {
        if (this == nullptr)
            return s;

        std::swap(*this, s);
        return *this;
    }

    friend void swap(Any &s, Any &r) { std::swap(s.p, r.p); }

    void clear() { delete p; p = nullptr; }

    bool empty() const { return p; }

    template<typename T>
    bool is() const { return p ? p->is(type_id<T>()) : false; }

    template<typename T> T      &&_() && { return std::move(stat<T>()); }
    template<typename T> T       &_()      & { return stat<T>(); }
    template<typename T> T const &_() const& { return stat<T>(); }

    template<typename T> T      &&cast() && { return std::move(dyn<T>()); }
    template<typename T> T       &cast()      & { return dyn<T>(); }
    template<typename T> T const &cast() const& { return dyn<T>(); }

    template<typename T> operator T && () && { return std::move(_<T>()); }
    template<typename T> operator T      &()      & { return _<T>(); }
    template<typename T> operator T const&() const& { return _<T>(); }
};


template <unsigned N>
struct apply
{
    template <typename Ret, typename... Args, typename... ArgsT>
    static Ret FuncCaller(Ret(func)(Args...), Any* v, ArgsT... args)
    {
        return apply<N - 1>::FuncCaller(func, v, args..., v[sizeof...(ArgsT)]);
    }
};

template<>
struct apply<0>
{
    template <typename Ret, typename... Args, typename... ArgsT>
    static Ret FuncCaller(Ret(func)(Args...), Any* v, ArgsT... args)
    {
        return func(args...);
    }
};

template <typename Ret, typename... Args>
static Ret FuncCaller(Ret(func)(Args...), Any* v)
{
    return apply<sizeof...(Args)>::FuncCaller(func, v);
}

template<typename Ret, typename ... Param>
struct Func_Caller
{
    explicit Func_Caller(Ret(*t_func)(Param...)) : m_func(t_func) {}

    template<typename ... Inner>
    Ret operator()(Inner&& ... inner) const {
        return (m_func)(std::forward<Inner>(inner)...);
    }

    Ret(*m_func)(Param...);
};

class Caller
{
public:
    virtual Any Call(Any* v) = 0;
};

template<typename Callable>
class CallerImpl : public Caller
{
public:
    CallerImpl(Callable f) : m_f(std::move(f)) { }

    Any Call(Any* v) override
    {
        return FuncCaller(m_f, v);
    }
    Callable m_f;
};
#endif
