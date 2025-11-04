#ifndef MYSTL_H
#define MYSTL_H

#include <deque>
#include <cstddef>
#include <stdexcept>

template <typename T>
class MyDeque
{
public:
    explicit MyDeque(size_t maxSize = 10)
        : m_maxSize(maxSize)
    {
    }

    void push_back(const T& value)
    {
        if (m_data.size() >= m_maxSize)
        {
            m_data.pop_front();
        }
        m_data.push_back(value);
    }

    void pop_front()
    {
        if (!m_data.empty()) {
            m_data.pop_front();
        }
    }

    T front() const
    {
        if (m_data.empty())
        {
            throw std::out_of_range("MyDeque is empty");
        }
        return m_data.front();
    }

    size_t size() const
    {
        return m_data.size();
    }

    bool empty() const
    {
        return m_data.empty();
    }

    void clear()
    {
        m_data.clear();
    }


    bool full() const
    {
        return m_data.size() >= m_maxSize;
    }

    void fill(const T& value)
    {
        while (m_data.size() < m_maxSize - 1) {
            m_data.push_back(value);
        }
    }
private:
    size_t m_maxSize; 
    std::deque<T> m_data;
    
};

#endif // MYSTL_H