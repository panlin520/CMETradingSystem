#pragma once

/*
====================================================================

price_level.hpp

CME Trading System
L3 OrderBook - Price Level FIFO

职责：

    保存同一个价格上的所有订单。

例如：

    ES Bid 6000.00

    Order 10001
          ↓
    Order 10002
          ↓
    Order 10003

这里体现 CME 的时间优先原则：

    First In First Out

先挂出的订单优先。

====================================================================
*/

#include <cstdint>
#include "order.hpp"

namespace trading
{
namespace orderbook
{

class PriceLevel
{
public:

    PriceLevel() = default;


    /*
    ================================================================
    添加订单到队列尾部。

    新订单永远排在最后。

    时间优先：

        old order
             ↓
        new order

    ================================================================
    */
    void append(Order* order) noexcept
    {
        if (order == nullptr)
            return;

        order->prev = tail_;
        order->next = nullptr;

        if (tail_)
        {
            tail_->next = order;
        }
        else
        {
            head_ = order;
        }

        tail_ = order;

        volume_ += order->size;
        count_++;
    }


    /*
    ================================================================
    从 FIFO 中删除订单。

    因为 Order 保存 prev / next，
    不需要扫描整个队列。

    时间复杂度：

        O(1)

    ================================================================
    */
    void remove(Order* order) noexcept
    {
        if (order == nullptr)
            return;

        if (order->prev)
            order->prev->next = order->next;
        else
            head_ = order->next;

        if (order->next)
            order->next->prev = order->prev;
        else
            tail_ = order->prev;

        volume_ -= order->size;

        if (count_ > 0)
            count_--;

        order->prev = nullptr;
        order->next = nullptr;
    }


    uint64_t volume() const noexcept
    {
        return volume_;
    }


    uint64_t order_count() const noexcept
    {
        return count_;
    }


    Order* head() const noexcept
    {
        return head_;
    }


    Order* tail() const noexcept
    {
        return tail_;
    }


private:

    // FIFO 第一笔订单
    Order* head_{nullptr};

    // FIFO 最后一笔订单
    Order* tail_{nullptr};

    // 当前价格档总数量
    uint64_t volume_{0};

    // 当前价格档订单数量
    uint64_t count_{0};

};

} // namespace orderbook
} // namespace trading
