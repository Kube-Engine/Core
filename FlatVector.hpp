/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: FlatVector
 */

#include "Assert.hpp"
#include "Utils.hpp"

namespace kF::Core
{
    template<typename Type>
    class FlatVector;
}

template<typename Type>
class kF::Core::FlatVector
{
public:
    struct alignas(sizeof(Type) <= 16 ? 16 : 64) Header
    {
        std::size_t size { 0 };
        std::size_t capacity { 0 };
    };

    /** @brief Default constructor */
    FlatVector(void) noexcept = default;

    /** @brief Init constructor */
    template<typename ...Args>
    FlatVector(const std::size_t count, Args &&...args) noexcept(nothrow_ndebug && nothrow_constructible(Type, Args...) && nothrow_destructible(Type)) { resize(count, std::forward<Args>(args)...); }

    /** @brief Destroy constructor */
    ~FlatVector(void) noexcept_destructible(Type) { release(); }

    /** @brief Get begin / end iterators */
    [[nodiscard]] Type *begin(void) noexcept;
    [[nodiscard]] const Type *begin(void) const noexcept;
    [[nodiscard]] Type *end(void) noexcept;
    [[nodiscard]] const Type *end(void) const noexcept;

    /** @brief Get raw data pointer */
    [[nodiscard]] Type *data(void) noexcept { return reinterpret_cast<Type *>(_ptr + 1); }
    [[nodiscard]] const Type *data(void) const noexcept { return reinterpret_cast<const Type *>(_ptr + 1); }

    /** @brief Access an element of the vector */
    [[nodiscard]] Type &at(const std::size_t index) noexcept { return data()[index]; }
    [[nodiscard]] const Type &at(const std::size_t index) const noexcept { return data()[index]; }
    [[nodiscard]] Type &operator[](const std::size_t index) noexcept { return at(index); }
    [[nodiscard]] const Type &operator[](const std::size_t index) const noexcept { return at(index); }

    /** @brief Fast check if vector contains data */
    [[nodiscard]] bool empty(void) const noexcept { return !_ptr || !_ptr->size; }
    operator bool(void) const noexcept { return !empty(); }

    /** @brief Get size of vector */
    [[nodiscard]] std::size_t size(void) const noexcept;

    /** @brief Get capacity of vector */
    [[nodiscard]] std::size_t capacity(void) const noexcept;

    /** @brief Push an element into the vector */
    template<typename ...Args>
    void push(Args &&...args) noexcept(nothrow_ndebug && nothrow_constructible(Type, Args...));

    /** @brief Push the last element of the vector */
    void pop(void) noexcept_destructible(Type);

    /** @brief Resize the vector using default constructor to initialize each element */
    void resize(const std::size_t count) noexcept(nothrow_ndebug && nothrow_constructible(Type) && nothrow_destructible(Type));

    /** @brief Resize the vector with given arguments initializing each element */
    template<typename ...Args>
    void resize(const std::size_t count, Args &&...args) noexcept(nothrow_ndebug && nothrow_constructible(Type, Args...) && nothrow_destructible(Type));

    /** @brief Reserve memory for fast emplace */
    void reserve(const std::size_t count) noexcept(nothrow_ndebug && nothrow_destructible(Type));

    /** @brief Clear the vector */
    void clear(void) noexcept_destructible(Type);

    /** @brief Clear vector and release memory */
    void release(void) noexcept_destructible(Type);

private:
    Header *_ptr { nullptr };

    /** @brief Grow the vector */
    void grow(void) noexcept_ndebug;
};

#include "FlatVector.ipp"

static_assert(sizeof(kF::Core::FlatVector<int>::Header) == 16);
static_assert(sizeof(kF::Core::FlatVector<char[16]>::Header) == 16);
static_assert(sizeof(kF::Core::FlatVector<char[17]>::Header) == 64);