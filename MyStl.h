#ifndef MYSTL_H
#define MYSTL_H

#include <deque>

class MyDeque
{
public:
    explicit MyDeque(size_t maxSize = 10);

    void push_back(int value);   // 若满则自动 pop_front() 再 push_back()
    void pop_front();
    int front() const;
    size_t size() const;
    bool empty() const;
    void clear();

    // --- 扩展接口 ---
    bool full() const;           // 判断是否已满
    void fill(int value);        // 若未满则自动补满 value

private:
    size_t m_maxSize;
    std::deque<int> m_data;
};
#endif // MYSTL_H