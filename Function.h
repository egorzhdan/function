#ifndef FUNCTION_FUNCTION_H
#define FUNCTION_FUNCTION_H

#include <cstddef>
#include <any>
#include <memory>
#include <type_traits>

template<class T>
class Function;

template<class Ret, class ...Args>
class Function<Ret(Args...)> {
public:
    class Call {
    public:
        virtual Ret operator()(Args...) const = 0;
    };

    template<typename Callable>
    class WrappingCall : public Call {
    public:
        Callable c;

        WrappingCall(Callable c) : c(c) {}

        Ret operator()(Args... args) const override {
            return c(args...);
        }
    };

    Function() noexcept : call(nullptr) {}

    Function(std::nullptr_t) noexcept : call(nullptr) {}

    Function(const Function &other) : call(other.call) {}

    Function(Function &&other) noexcept : call(other.call) {}

    template<class F>
    Function(F f) : call([&] {
        if constexpr (std::is_constructible<Call *, F>::value) {
            return static_cast<Call *>(f);
        } else {
            return new WrappingCall<F>(f);
        }
    }()) {}

    ~Function() = default;

    Function &operator=(const Function &other) { call = other.call; }

    Function &operator=(Function &&other) noexcept { call.swap(other.call); }

    void swap(Function &other) noexcept { call.swap(other.call); }

    explicit operator bool() const noexcept { return call; }

    virtual Ret operator()(Args... args) const {
        if (call)
            return call->operator()(args...);
        else
            throw std::bad_function_call();
    }

private:
    std::shared_ptr<Call> call;
};

#endif //FUNCTION_FUNCTION_H
