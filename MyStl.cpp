#include "MyStl.h"

MyDeque::MyDeque(size_t maxSize)
    : m_maxSize(maxSize)
{
}

void MyDeque::push_back(int value)
{
    if (m_data.size() >= m_maxSize)
    {
        m_data.pop_front();
    }
    m_data.push_back(value);
}

void MyDeque::pop_front()
{
    if (!m_data.empty()) {
        m_data.pop_front();
    }
}

int MyDeque::front() const
{
    return m_data.front();
}

size_t MyDeque::size() const
{
    return m_data.size();
}

bool MyDeque::empty() const
{
    return m_data.empty();
}

void MyDeque::clear()
{
    m_data.clear();
}


bool MyDeque::full() const
{
    return m_data.size() >= m_maxSize;
}

void MyDeque::fill(int value)
{
    while (m_data.size() < m_maxSize) {
        m_data.push_back(value);
    }
}