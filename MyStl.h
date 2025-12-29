#ifndef MYSTL_H
#define MYSTL_H

#include <deque>
#include <cstddef>
#include <stdexcept>

template <typename T>
class MyDeque
{
public:
    explicit MyDeque(size_t maxSize = 20)
        : m_maxSize(maxSize)
    {
        if constexpr (std::is_same_v<T, int>)
        {
            m_data.resize(m_maxSize, 1);
        }
    }

    void push_back(const T& value)
    {
        if (m_data.size() >= m_maxSize)
        {
            m_data.pop_front();
        }

       if(polution_flag==false) m_data.push_back(value);
	   else if constexpr (std::is_same_v<T, int>)
       {
           polution_flag = false;
           m_data.push_back(0);
       }
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

    void resize(size_t newMaxSize)
    {
        m_maxSize = newMaxSize;

        while (m_data.size() > m_maxSize)
        {
            m_data.pop_front();
        }

        // int 类型：重新初始化为全 1
        if constexpr (std::is_same_v<T, int>)
        {
            m_data.clear();
            m_data.resize(m_maxSize, 1);
        }
        else
        {
            // 非 int 类型：保持现有数据，不做额外处理
        }
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

    void polution()//for flower
    {
        if (m_data.empty())
        {
            if constexpr (std::is_same_v<T, int>)
            {
                m_data.push_back(0);
            } 
        }
        else
        {
            if constexpr (std::is_same_v<T, int>)
            {
                m_data.push_back(0);
            }
        }
        polution_flag = true;
    }

public:
    size_t m_maxSize; 
    std::deque<T> m_data;
	bool polution_flag = false;
    long long time_id;
};

#endif // MYSTL_H