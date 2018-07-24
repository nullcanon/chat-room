#pragma once

#include<vector>
#include<cstdlib>
#include<semaphore.h>

namespace server
{
    // 内置同步互斥机制
    // 应对多线程读写
    // 如队列为空，尝试读，读操作阻塞
    // 如队列为满，尝试写，写操作阻塞

template<typename T>
class BlockingQueue
{
public:
    // s为队列中最大元素个数
    BlockingQueue(size_t s = 1024)
        :_data(s)
        ,_head(0)
        ,_tail(0)
        ,_size(0)
    {
        sem_init(&_lock,0,1);
        sem_init(&_data_sem,0,0);
        sem_init(&_blank_sem,0,s);
    }

    ~BlockingQueue()
    {
        sem_destory(&_lock);
        sem_destory(&_data_sem);
        sem_destory(&_blank_sem);
    }

    // 入队列
    void PushBack(const T& msg)
    {
        // 先申请一个空白资源
        sem_wait(&_blank_sem);

        sem_wait(&_lock);
        _data[_tail++] = msg;
        if(_tail >= _data.siz())
        {
            _tail = 0;
        }
        ++_size;
        sem_post(&_lock);

        sem_post(&_data_sem);
    }

    // 出队列
    // 取队首元素
    void PopFront(T* msg)
    {
        sem_wait(&_data);

        sem_wait(&_lock);
        *msg = _data[_head++];
        if(_head >= _data.size())
        {
            _head = 0;
        }
        --_size;
        sem_post(&_lock);

        sem_post(&_blank_sem);
    }


private:
    std::vector<T> _data;

    size_t _head;
    size_t _tail;
    size_t _size;

    sem_t _lock;    // 二元信号量，用于互斥
    sem_t _data_sem;    // 表示可用元素的个数
    sem_t _blank_sem;  // 表示空白资源的个数
};

}// end server
