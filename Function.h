#ifndef FUNCTION_FUNCTION_H
#define FUNCTION_FUNCTION_H

#include <cstddef>
#include <any>
#include <memory>
#include <vector>
#include <type_traits>

#define SMALL_SIZE 8

template<class T>
class Function;

template<class Ret, class ...Args>
class Function<Ret(Args...)> {
public:
    class Call {
    public:
        virtual Ret operator()(Args &&...) const = 0;

        virtual void cloneInPlace(char *dest) = 0;

        virtual std::unique_ptr<Call> clone() = 0;
    };

    template<typename Callable>
    class WrappingCall : public Call {
    public:
        Callable c;

        WrappingCall(Callable c) : c(c) {}

        Ret operator()(Args &&... args) const override {
            return c(std::forward<Args>(args)...);
        }

        void cloneInPlace(char *dest) override {
            new(dest) WrappingCall<Callable>(*this);
        }

        std::unique_ptr<Call> clone() override {
            return std::make_unique<WrappingCall<Callable>>(c);
        }
    };

    Function() noexcept : call(nullptr) {}

    Function(std::nullptr_t) noexcept : call(nullptr) {}

    Function(const Function &other) : is_small(other.is_small) {
        if (is_small) {
            other.call->clone(small);
        } else {
            call = other.call->clone();
        }
    }

    Function(Function &&other) noexcept : is_small(other.is_small) {
        if (is_small) {
            std::swap(small, other.small);
        } else {
            std::swap(call, other.call);
        }
    }

    template<class F>
    Function(F f) {
        if (sizeof(f) < SMALL_SIZE * sizeof(char)) {
            is_small = true;
            new(small) WrappingCall<F>(f);
        } else {
            is_small = false;
            call = std::make_unique<WrappingCall<F>>(f);
        }
    }

    ~Function() {
        if (!is_small) {
            call.reset();
        } else {
            get()->~Call();
        }
    }

    Function &operator=(const Function &other) {
        if (this == other) return *this;
        is_small = other.is_small;
        if (is_small) {
            new(small) WrappingCall(other.get());
        } else {
            call = other.call->clone();
        }
        return *this;
    }

    Function &operator=(Function &&other) noexcept {
        call.swap(other.call);
        return *this;
    }

    void swap(Function &other) noexcept {
        std::swap(is_small, other.is_small);
        std::swap(small, other.small);
    }

    explicit operator bool() const noexcept { return get(); }

    virtual Ret operator()(Args &&... args) const {
        auto *f = get();
        if (f)
            return f->operator()(std::forward<Args>(args)...);
        else
            throw std::bad_function_call();
    }

private:
    bool is_small = false;
    union {
        std::unique_ptr<Call> call;
        char small[SMALL_SIZE];
    };

    inline Call *get() const {
        if (is_small) return (Call *) &small;
        return call.get();
    }
};

#endif //FUNCTION_FUNCTION_H
