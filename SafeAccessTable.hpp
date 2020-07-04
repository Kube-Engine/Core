/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SafeAccessTable
 */

#include <vector>
#include <array>
#include <shared_mutex>
#include <memory>

namespace kF
{
    template<typename Key, typename Value>
    class SafeAccessTable;
}

/**
 * @brief The SafeAccessTable is a look-up table that synchronize access of its values
 */
template<typename Key, typename Value>
class kF::SafeAccessTable
{
public:
    struct Entry
    {
        template<typename ...Args>
        Entry(Args &&...args) : value(std::forward<Args>(args)...) {}

        ~Entry(void) = default;

        Value value;
        std::mutex valueMutex {};
    };

    struct Row
    {
        Key key;
        std::unique_ptr<Entry> entry;
    };

    using Table = std::vector<Row>;

    class Holder
    {
    public:
        Holder(void) noexcept = default;
        Holder(Row *row) noexcept : _row(row) {}
        Holder(Holder &&other) noexcept : _row(other._row) { other._row = nullptr; }
        ~Holder(void) noexcept { if (_row) _row->entry->valueMutex.unlock(); }
        Holder &operator=(Holder &&other) noexcept { std::swap(_row, other._row); return *this; }

        operator bool(void) const noexcept { return _row; }

        [[nodiscard]] Key &key(void) const noexcept { return _row->key; }
        [[nodiscard]] Value &value(void) const noexcept { return _row->entry->value; }

    private:
        Row *_row = nullptr;
    };

    template<typename ...Args>
    void insert(const Key &key, Args &&...args) noexcept(std::is_nothrow_constructible_v<Value>)
    {
        auto writeLock = std::lock_guard(_tableMutex);

        _table.emplace_back(Row {
            key: key,
            entry: std::make_unique<Entry>(std::forward<Args>(args)...)
        });
    }

    template<typename ...Args>
    void tryInsert(const Key &key, Args &&...args)
    {
        auto writeLock = std::lock_guard(_tableMutex);
        for (auto &row : _table) {
            if (row.key != key)
                continue;
            if constexpr (std::is_move_assignable_v<Value>)
                row.entry->value = Value(std::forward<Args>(args)...);
            else {
                row.entry->value.~Value();
                new (&row.entry->value) Value(std::forward<Args>(args)...);
            }
            return;
        }
        _table.emplace_back(Row {
            key: key,
            entry: std::make_unique<Entry>(std::forward<Args>(args)...)
        });
    }

    [[nodiscard]] Holder find(const Key &key) noexcept
    {
        for (auto readLock = std::shared_lock(_tableMutex); auto &row : _table) {
            if (row.key != key)
                continue;
            return Holder(&row);
        }
        return Holder();
    }

    void erase(const Key &key) noexcept(std::is_nothrow_destructible_v<Value>)
    {
        auto writeLock = std::lock_guard(_tableMutex);

        for (auto it = _table.begin(); it != _table.end(); ++it) {
            if (it->key != key)
                continue;
            _table.erase(it);
            break;
        }
    }

private:
    Table _table;
    std::shared_mutex _tableMutex;
};