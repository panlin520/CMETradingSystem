#pragma once

/*
====================================================================
CME Trading System
L3 OrderBook - Order Definition

职责:
    保存 CME MBO/L3 中的单个订单状态。

说明:
    L3 数据不是只有价格和数量，而是每一个订单都有唯一
    order_id。因此订单簿必须保存订单级别信息。

    Order 本身只保存数据，不处理 Add/Modify/Cancel。
    生命周期管理由 L3OrderBook 负责。

设计:

    PriceLevel FIFO

    Order A
       |
       v
    Order B
       |
       v
    Order C

    保证 CME 的价格优先 + 时间优先规则。
====================================================================
*/

#include <cstdint>

namespace trading
{
namespace orderbook
{

// 买卖方向
// 对应 Databento MBO side 字段
// B = Bid
// A = Ask
//
enum class OrderSide
{
    BID,
    ASK
};

// 订单当前状态
enum class OrderStatus
{
    ACTIVE,
    CANCELLED,
    FILLED
};

// 单个市场订单
struct Order
{
    // 交易所订单唯一编号
    uint64_t order_id{0};

    // 买卖方向
    OrderSide side{OrderSide::BID};

    // 原始价格
    // 使用整数，避免 double 精度问题
    int64_t price{0};

    // 当前剩余数量
    int64_t size{0};

    // Databento sequence，用于回放一致性检查
    uint64_t sequence{0};

    // 当前生命周期状态
    OrderStatus status{OrderStatus::ACTIVE};

    // intrusive FIFO 链表指针
    // 同价格订单按照进入顺序排列
    Order* prev{nullptr};
    Order* next{nullptr};

    // 判断订单是否仍然存在盘口
    bool active() const noexcept
    {
        return status == OrderStatus::ACTIVE;
    }
};

} // namespace orderbook
} // namespace trading
