#ifndef QUEUE_H
#define QUEUE_H

#include <cstddef>
#include <stdexcept>
#include <vector>

template <typename T>
class Queue
{
private:
    std::vector<T> items;
    std::size_t head;
    std::size_t itemCount;

public:
    explicit Queue(std::size_t capacity)
        : items(capacity), head(0), itemCount(0)
    {
        if (capacity == 0)
        {
            throw std::invalid_argument("Capacity must be positive");
        }
    }

    bool enqueue(const T &value)
    {
        if (full())
        {
            return false;
        }

        // Find the next available position.
        std::size_t position = (head + itemCount) % items.size();

        items[position] = value;
        itemCount++;

        return true;
    }

    bool dequeue(T &value)
    {
        if (empty())
        {
            return false;
        }

        value = items[head];

        // Move to the next position.
        head++;

        // Wrap back to the beginning.
        if (head == items.size())
        {
            head = 0;
        }

        itemCount--;
        return true;
    }

    bool front(T &value) const
    {
        if (empty())
        {
            return false;
        }

        value = items[head];
        return true;
    }

    bool empty() const
    {
        return itemCount == 0;
    }

    bool full() const
    {
        return itemCount == items.size();
    }

    std::size_t size() const
    {
        return itemCount;
    }

    std::size_t capacity() const
    {
        return items.size();
    }
};

#endif