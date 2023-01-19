#include <U2Core/U2SafePoints.h>

/**
 * Helper class to handle nullable pointer values in a more safe (than default) and explicit way.
 * There are only 2 methods that can access the original pointer value:
 *
 * 1. get(): asserts that the value is not null and returns the value.
 *  A user can assert that the value is not null only if it was previously checked: isNullptr() is called.
 * 2. asNullable(): returns the original pointer value with no checks.
 */
template<typename T>
class Nullable {
public:
    Nullable(T* _value = nullptr)
        : value(_value) {
    }

    /** Returns true if the pointer value is not null. */
    bool isNullptr() const {
        isChecked = true;
        return value != nullptr;
    }

    bool operator==(const T* anotherValue) const {
        isChecked = isChecked || anotherValue == nullptr;
        return anotherValue == value;
    }

    operator T*() const {
        return get();
    }

    /** Returns pointer value that is guaranteed to be not null. Requires that isNullptr() or ==() is called before this method. */
    T* get() const {
        SAFE_POINT(isChecked, "Nullable::get is called with null checks for null!", value);
        SAFE_POINT(value != nullptr, "Nullable::get is called for a nullptr value!", value);
        return value;
    }

private:
    T* value;
    mutable bool isChecked = false;
};
