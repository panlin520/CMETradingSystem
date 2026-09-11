# ESU26 C++ + Python Web UI 企业级交易系统项目规范

> 项目定位：基于 **Databento 官方 CME L3 深度数据**，构建面向 **ESU26** 的企业级交易研究、回测、模拟交易与实盘交易系统。
>
> 核心原则：**交易核心使用 C++，Python 仅作为交互、研究辅助和 Web UI 桥接层。**
>
> 研发顺序：**先在 Windows + Visual Studio 2026 完成开发、测试与高强度验证，再迁移至 Linux 企业级运行环境。**
>
> 本文档是项目级硬约束。后续任何代码创建、修改、扩展、重构，都必须遵守本文档。

---

# 1. 项目目标

构建完整闭环：

```text
Databento CME L3
        ↓
Market Data Feed
        ↓
Replay / Live Dispatcher
        ↓
L3 OrderBook
        ↓
Feature Engine
        ↓
Strategy Engine
        ↓
Risk Manager
        ↓
Order Manager
        ↓
Execution Engine
        ↓
Fill
        ↓
Portfolio / Position / PnL
        ↓
Metrics / Monitoring
        ↓
Web UI
```

系统必须支持：

```text
BACKTEST
PAPER
LIVE
```

三种模式共用统一核心接口和统一业务逻辑。

---

# 2. 技术路线

## 2.1 C++ 负责核心交易系统

C++ 必须负责：

```text
Market Data
Replay Engine
Event Dispatcher
L3 OrderBook
Feature Engine
Strategy Runtime
Risk
Order Management
Execution
Position
Portfolio
PnL
Clock
Statistics
Persistence Interface
System State
```

核心交易链路不得依赖 Python 才能运行。

---

## 2.2 Python 负责交互与 Web UI

Python 负责：

```text
Web Backend
WebSocket
REST API
Research Tools
Offline Analysis
Visualization
Configuration UI
Monitoring UI
Control UI
Python SDK / Bridge
```

Python 不负责：

```text
L3 OrderBook 核心重建
真实撮合核心逻辑
核心风控
核心订单状态机
实盘交易核心状态
```

---

## 2.3 Web UI

建议架构：

```text
C++ Trading Core
        ↓
IPC / TCP / gRPC / Shared API
        ↓
Python Service
        ↓
FastAPI
        ↓
WebSocket / REST
        ↓
Web Frontend
```

前端技术可选：

```text
React
Vue
```

具体实现前必须先确认项目现有接口。

---

# 3. 硬性开发规则

## 3.1 写代码前必须先查询接口

这是本项目最高优先级规则之一。

任何以下操作：

```text
修改代码
新增代码
创建文件
升级模块
重构模块
替换接口
```

之前，必须先查询并确认：

```text
当前对应文件
当前类定义
当前函数签名
当前 include
当前 namespace
当前调用方
当前被调用方
当前数据结构
当前测试
当前依赖
当前 GitHub 仓库状态
```

禁止：

```text
凭记忆修改
凭旧版本修改
猜测接口
假设类存在
假设函数签名
创建重复模块
绕过现有架构
```

---

## 3.2 必须先读取对应源码

例如准备修改：

```text
execution_engine.cpp
```

必须先读取：

```text
execution_engine.hpp
execution_engine.cpp
order.hpp
fill.hpp
risk_manager.hpp
portfolio.hpp
调用 execution_engine 的代码
相关 tests
```

确认真实接口之后才能修改。

---

## 3.3 每次修改必须提供完整文件

禁止：

```text
只给 diff
只给函数片段
只给补丁
只给几行修改
“把这一段替换进去”
```

必须提供：

```text
完整 .hpp
完整 .cpp
完整 .py
完整配置文件
完整测试文件
```

若文件过长，可分：

```text
1/2
2/2
```

但必须组成完整可运行文件。

---

## 3.4 不允许破坏已经通过的基线

一旦某个模块已经通过稳定测试：

```text
BASELINE = PASSED
```

除非发现真实 Bug，否则不得随意修改。

若必须修改：

```text
先读取当前代码
先读取当前测试
说明修改原因
说明影响模块
提供完整文件
重新执行回归测试
```

---

# 4. 系统模式

统一定义：

```cpp
enum class EngineMode
{
    Backtest,
    Paper,
    Live
};
```

三种模式必须使用同一套：

```text
Event
OrderBook
Feature
Strategy
Risk
Order
Portfolio
```

区别只允许出现在：

```text
Feed
Clock
Execution Adapter
Broker Adapter
Persistence / Replay Source
```

---

# 5. BACKTEST 模式

数据源：

```text
Databento 历史 L3
```

流程：

```text
DBN
 ↓
Historical Feed
 ↓
Replay Engine
 ↓
L3 OrderBook
 ↓
Feature
 ↓
Strategy
 ↓
Risk
 ↓
Simulated Execution
 ↓
Fill
 ↓
Portfolio
```

必须支持：

```text
事件顺序重放
纳秒时间戳
加速回放
单步回放
指定区间
多数据文件
多交易日
确定性重放
```

同一输入必须得到同一输出。

---

# 6. PAPER 模式

数据：

```text
实时 Databento L3
```

执行：

```text
模拟撮合
```

流程：

```text
Live Feed
 ↓
L3 OrderBook
 ↓
Feature
 ↓
Strategy
 ↓
Risk
 ↓
Paper Execution
 ↓
Fill
 ↓
Portfolio
```

PAPER 模式必须尽可能复用 BACKTEST 的撮合接口。

---

# 7. LIVE 模式

流程：

```text
Live Feed
 ↓
L3 OrderBook
 ↓
Feature
 ↓
Strategy
 ↓
Risk
 ↓
Live Execution
 ↓
Broker / Gateway
 ↓
Exchange
 ↓
Execution Report
 ↓
Fill
 ↓
Portfolio
```

LIVE 模式不得：

```text
伪造成交
假 Fill
绕过 Broker 回报
绕过 Risk
```

真实仓位变化必须来自真实成交回报。

---

# 8. Databento 数据要求

数据源：

```text
Databento 官方数据
CME
L3 / MBO 深度数据
ESU26
```

正式开发前必须确认：

```text
Dataset
Schema
Symbol
Contract Month
Price Scale
Timestamp Units
Action Semantics
Side Semantics
Flags
Sequence Semantics
Snapshot / Replay Semantics
```

禁止把这些内容写死为猜测。

必须以：

```text
Databento 官方接口
官方 Schema
当前实际数据文件
```

为 Ground Truth。

---

# 9. Market Data 层

建议接口：

```cpp
struct MarketEvent;
```

所有 Feed 输出统一事件结构。

禁止：

```text
不同模式使用不同 MarketEvent
BACKTEST 一套事件
LIVE 另一套事件
```

统一：

```text
HistoricalFeed
LiveFeed
        ↓
MarketEvent
```

---

# 10. L3 OrderBook 是系统核心

OrderBook 是整个系统最高可靠性模块之一。

目标：

```text
MBO Events
   ↓
Exact L3 Reconstruction
   ↓
Bid / Ask Price Levels
   ↓
FIFO Orders
```

必须支持：

```text
Add
Modify
Cancel
Replace
Trade
Fill
Reset
Snapshot
```

具体 Action 必须以 Databento 当前 Schema 为准。

---

# 11. OrderBook 数据结构

建议分层：

```text
OrderBook
 ├── Bid Side
 ├── Ask Side
 ├── PriceLevel
 │    ├── FIFO Order
 │    ├── FIFO Order
 │    └── FIFO Order
 └── OrderId Index
```

核心目标：

```text
O(1) OrderId 查找
高效 PriceLevel 更新
FIFO 正确
低分配
低锁竞争
低延迟
确定性
```

---

# 12. OrderBook 可靠性要求

OrderBook 必须经过多轮高强度测试。

最低要求：

## Unit Test

```text
Add
Modify
Cancel
Replace
Trade
Fill
Reset
Empty Book
Single Level
Multi Level
Duplicate OrderId
Unknown OrderId
Zero Size
Invalid Side
Invalid Price
```

---

## FIFO Test

验证：

```text
同价格
不同 OrderId
不同时间顺序
```

必须严格保持 FIFO。

---

## Snapshot Test

验证：

```text
Snapshot 初始化
Snapshot 后增量更新
Snapshot 与直接重放一致
```

---

## Ground Truth Test

使用 Databento 官方数据与可验证输出进行对照。

要求：

```text
Best Bid
Best Ask
Depth
Order Count
Volume
Trade
Sequence
```

逐项验证。

---

## Long Replay Test

至少：

```text
100K
1M
5M
Full Session
Full Day
Multi Day
```

事件。

---

## Stress Test

重复：

```text
10 次
50 次
100 次
```

执行相同 Replay。

要求：

```text
无随机崩溃
无状态漂移
无内存增长
无结果漂移
```

---

## Sanitizer Test

Linux 阶段必须加入：

```text
AddressSanitizer
UndefinedBehaviorSanitizer
ThreadSanitizer
```

Windows 可增加：

```text
Visual Studio AddressSanitizer
```

---

# 13. Replay Engine

Replay 必须支持：

```text
Pause
Resume
Step
Speed
Seek
Start Time
End Time
Event Counter
Replay Statistics
```

核心要求：

```text
确定性
顺序正确
时间正确
状态可复现
```

---

# 14. Feature Engine

FeatureEngine 必须完全基于已重建的 OrderBook 和交易事件。

禁止：

```text
绕过 OrderBook
直接从原始 DBN 随意计算策略
```

建议功能：

```text
Best Bid / Ask
Spread
Mid Price
Micro Price
Depth
OBI
OFI
Trade Flow
Aggressive Buy/Sell
Cancel Rate
Add Rate
Refill
Queue Depletion
Liquidity Vacuum
Absorption
Volatility
Regime
```

---

# 15. Strategy Engine

Strategy 不允许直接操作：

```text
Broker
Execution Adapter
Portfolio 内部状态
Raw Socket
Raw DBN
```

Strategy 输入：

```text
StrategyContext
```

输出：

```text
Signal
```

统一接口。

---

# 16. Signal

统一 Signal：

```text
Symbol
Side
Quantity / Target
Score
Confidence
StrategyId
Reason
Timestamp
```

Signal 不等于 Order。

流程：

```text
Strategy
 ↓
Signal
 ↓
Risk
 ↓
Order
```

---

# 17. Risk Manager

Risk 必须位于：

```text
Signal
 ↓
Risk
 ↓
Order
```

支持：

```text
Max Position
Max Order Size
Daily Loss
Max Drawdown
Exposure
Consecutive Loss
Spread Filter
Liquidity Filter
Volatility Filter
Slippage
Order Rate
Latency
Data Integrity
Execution Failure
Margin
Buying Power
Kill Switch
```

LIVE 模式必须具备强制 Kill Switch。

---

# 18. Order Manager

Order 状态机必须统一。

建议：

```text
Created
Submitted
Accepted
PartiallyFilled
Filled
CancelPending
Cancelled
Rejected
Expired
```

订单对象不得与 L3 OrderBook 内部订单混用。

必须明确区分：

```text
MarketDataOrder
TradingOrder
```

---

# 19. Execution Engine

Execution 负责：

```text
Order
 ↓
Execution
 ↓
Fill
```

不负责：

```text
Strategy
PnL
Portfolio 决策
```

模式通过 Adapter 统一：

```text
BacktestExecutionAdapter
PaperExecutionAdapter
LiveExecutionAdapter
```

对上层暴露统一接口。

---

# 20. Portfolio / Position / PnL

统一流程：

```text
Fill
 ↓
Portfolio
 ↓
Position
 ↓
PnL
```

所有仓位变化必须来自 Fill。

Portfolio 不允许自行伪造成交。

---

# 21. 完整交易闭环

最终运行链：

```text
Databento L3
      ↓
Feed
      ↓
Event Dispatcher
      ↓
OrderBook
      ↓
Feature Engine
      ↓
Strategy
      ↓
Signal
      ↓
Risk
      ↓
Trading Order
      ↓
Execution
      ↓
Fill
      ↓
Portfolio
      ↓
PnL
      ↓
Risk Feedback
      ↓
Monitoring
      ↓
Web UI
```

---

# 22. C++ 与 Python 交互

原则：

```text
C++ = Source of Truth
Python = Client / Control / Visualization
```

Python 不保存独立交易真相状态。

状态必须来源于 C++。

建议接口层：

```text
C++ Core
   ↓
Unified API
   ↓
Python Bridge
```

可选技术：

```text
pybind11
gRPC
ZeroMQ
TCP
WebSocket Gateway
Shared Memory
```

具体选型必须在确认实时性、部署方式和现有接口后决定。

---

# 23. Web UI

建议显示：

## System

```text
Mode
Connection
Engine Status
Feed Status
Latency
Event Rate
CPU
Memory
```

## Market

```text
DOM
Best Bid / Ask
Spread
Depth
Time & Sales
Order Flow
```

## Strategy

```text
Strategy Status
Signal
Score
Confidence
Regime
```

## Risk

```text
Limits
Exposure
Daily Loss
Drawdown
Kill Switch
```

## Execution

```text
Orders
Fills
Rejects
Latency
Slippage
```

## Portfolio

```text
Position
Average Price
Realized PnL
Unrealized PnL
Equity
```

---

# 24. DOM UI

目标：

企业级实时 DOM。

必须：

```text
增量更新
不全量重绘
低延迟
状态一致
```

展示：

```text
Bid
Ask
Price
Depth
Order Count
Last Trade
Time & Sales
Aggressive Buy/Sell
Large Orders
Absorption
```

---

# 25. Windows 开发环境

第一阶段：

```text
Windows
Visual Studio 2026
MSVC
CMake
C++20 / C++23
```

具体语言标准必须项目统一，不允许模块自行选择。

建议：

```text
CMake
vcpkg 或 Conan
GoogleTest / Catch2
Benchmark
```

最终依赖管理方案确定后全项目统一。

---

# 26. Windows 测试要求

在迁移 Linux 前必须完成：

```text
Debug Build
Release Build
Unit Tests
Integration Tests
Stress Tests
Long Replay
Full Day Replay
Repeated Replay
Memory Test
Performance Benchmark
```

必须做到：

```text
Windows Baseline = PASSED
```

才允许进入 Linux 迁移。

---

# 27. Linux 企业级迁移

目标平台：

```text
Linux x86_64
```

建议：

```text
Ubuntu LTS / Rocky Linux
```

实际发行版在部署阶段确定。

Linux 使用：

```text
GCC / Clang
CMake
Ninja
systemd
Docker（可选）
```

---

# 28. Windows → Linux 跨平台规则

禁止核心代码依赖：

```text
Windows-only API
硬编码 Windows Path
Sleep 精度假设
平台特定 Socket
平台特定线程 API
```

统一封装：

```text
Filesystem
Clock
Thread
Socket
Process
Logging
Config
```

优先使用：

```text
C++ Standard Library
跨平台库
```

---

# 29. 企业级可靠性

系统必须具备：

```text
Structured Logging
Crash Diagnostics
Metrics
Health Check
Watchdog
Kill Switch
Config Validation
Data Validation
State Validation
Recovery
Graceful Shutdown
```

---

# 30. 日志

日志必须区分：

```text
TRACE
DEBUG
INFO
WARN
ERROR
CRITICAL
```

核心交易路径避免同步磁盘写导致延迟。

建议异步日志。

---

# 31. Metrics

至少：

```text
events/sec
book updates/sec
strategy evaluations/sec
orders/sec
fills/sec
latency
queue depth
memory
CPU
PnL
drawdown
rejects
errors
```

---

# 32. 性能原则

第一阶段：

```text
正确性 > 性能
```

当 Ground Truth 全部通过后再优化。

禁止：

```text
为了快破坏 FIFO
为了快跳过事件
为了快修改事件顺序
为了快使用不可靠缓存
```

---

# 33. 性能优化方向

确认正确后：

```text
Pre-allocation
Memory Pool
Object Pool
Flat Containers
Cache Friendly Layout
Reduced Allocations
Lock Reduction
Zero Copy
Batch Processing
CPU Affinity
NUMA Awareness
```

必须 Benchmark 后再采用。

---

# 34. 测试分层

```text
Level 1 Unit
Level 2 Contract
Level 3 Integration
Level 4 Ground Truth
Level 5 Replay
Level 6 Stress
Level 7 Soak
Level 8 Fault Injection
Level 9 Paper
Level 10 Live Qualification
```

---

# 35. 高强度测试标准

OrderBook 和 Runtime Pipeline 必须满足：

```text
重复运行结果一致
长时间无 Crash
无 Memory Leak
无 Undefined Behavior
无数据漂移
无事件丢失
无重复成交
无非法仓位
无负数量
无 crossed book（除非真实数据语义允许且已解释）
```

---

# 36. Fault Injection

必须测试：

```text
Feed Disconnect
Reconnect
Bad Event
Unknown OrderId
Duplicate Event
Out-of-order Event
Execution Reject
Broker Disconnect
Timeout
Slow Consumer
Queue Overflow
Disk Failure
UI Disconnect
```

核心交易线程不得因为 Web UI 断线而停止。

---

# 37. GitHub 规范

代码会上传 GitHub。

建议：

```text
main
develop
feature/*
fix/*
release/*
```

实际分支策略以当前仓库为准。

写代码前必须先确认：

```text
当前 branch
最新 commit
当前文件
当前接口
当前 tests
```

---

# 38. GitHub 提交要求

每次修改至少说明：

```text
修改文件
修改原因
接口变化
影响模块
测试结果
```

禁止：

```text
一次 Commit 混合大量无关修改
无测试提交核心 OrderBook 修改
未经确认删除重复文件
```

---

# 39. CI

建议 GitHub Actions：

```text
Windows Build
Windows Test
Linux Build
Linux Test
Static Analysis
Sanitizer
Formatting
```

核心分支必须要求 CI PASS。

---

# 40. 静态分析

建议：

```text
clang-tidy
cppcheck
MSVC Code Analysis
```

必须逐步清理：

```text
Undefined Behavior
Lifetime Issues
Unchecked Return
Narrowing
Race Risk
Null Dereference
```

---

# 41. C++ 编码规则

核心要求：

```text
代码简洁
职责单一
接口统一
命名统一
所有权清晰
生命周期清晰
错误处理统一
```

避免：

```text
God Class
巨型 Engine
跨层调用
循环依赖
隐藏全局状态
随意 Singleton
```

---

# 42. 接口统一

统一风格：

```cpp
start()
stop()
reset()
on_event(...)
snapshot()
stats()
```

具体接口以项目当前真实代码为准。

任何统一接口调整都必须先检查全部调用方。

---

# 43. 错误处理

必须区分：

```text
Recoverable
Non-Recoverable
Data Error
Execution Error
Risk Error
System Error
```

LIVE 模式严重错误应进入安全状态，而不是继续盲目交易。

---

# 44. 配置

配置统一管理。

禁止模块各自硬编码：

```text
Symbol
Tick Size
Point Value
Risk Limit
Endpoint
Port
Path
```

所有配置启动时验证。

---

# 45. ESU26 合约配置

正式实现前必须查询并确认：

```text
Instrument
Tick Size
Point Value
Contract Metadata
Trading Session
Databento Symbol Mapping
```

禁止凭旧合约配置直接复制。

---

# 46. 推荐目录结构

```text
TradingSystem/
│
├── CMakeLists.txt
├── cmake/
├── config/
├── docs/
│
├── include/
│   └── trading/
│       ├── core/
│       ├── market_data/
│       ├── orderbook/
│       ├── features/
│       ├── strategy/
│       ├── risk/
│       ├── order/
│       ├── execution/
│       ├── portfolio/
│       ├── broker/
│       └── common/
│
├── src/
│   ├── core/
│   ├── market_data/
│   ├── orderbook/
│   ├── features/
│   ├── strategy/
│   ├── risk/
│   ├── order/
│   ├── execution/
│   ├── portfolio/
│   ├── broker/
│   └── app/
│
├── python/
│   ├── bridge/
│   ├── web/
│   ├── research/
│   └── tools/
│
├── web/
│   ├── frontend/
│   └── static/
│
├── tests/
│   ├── unit/
│   ├── contract/
│   ├── integration/
│   ├── ground_truth/
│   ├── stress/
│   └── replay/
│
├── benchmarks/
├── tools/
├── scripts/
├── third_party/
└── .github/
    └── workflows/
```

该目录仅为目标结构。

正式创建目录/文件前仍必须检查 GitHub 当前仓库结构，不能直接覆盖现有项目。

---

# 47. 线程架构原则

初期保持简单。

建议逻辑：

```text
Market Data Thread
        ↓
Core Event Pipeline
        ↓
Strategy / Risk / Execution
```

UI / Logging / Persistence 与核心交易路径隔离。

不要一开始过度多线程。

在 Benchmark 和 Profiling 证明有必要后再拆线程。

---

# 48. Python 与 Core 隔离

任何 Python 崩溃：

```text
不得直接导致 C++ Core 崩溃
```

任何 Web UI 断线：

```text
不得直接影响交易核心
```

任何慢客户端：

```text
不得阻塞 Market Data Thread
```

---

# 49. 企业级实盘安全

LIVE 上线前必须达到：

```text
Backtest PASS
Paper PASS
Long Soak PASS
Risk PASS
Disconnect PASS
Reconnect PASS
Broker Reject PASS
Kill Switch PASS
Restart Recovery PASS
```

未完成不得进入正式资金实盘。

---

# 50. 开发阶段

## Phase 0：仓库核对

```text
读取 GitHub
读取目录
读取接口
读取 Tests
建立 Ground Truth
```

---

## Phase 1：C++ 基础框架

```text
Core
Event
Clock
Config
Logging
Mode
```

---

## Phase 2：Databento

```text
Historical Feed
MarketEvent
Replay
```

---

## Phase 3：L3 OrderBook

```text
Order
PriceLevel
Book
Builder
Snapshot
Validation
```

必须高强度验证。

---

## Phase 4：Feature

```text
OrderBook Feature
Trade Flow
Order Flow
Regime
```

---

## Phase 5：Strategy

```text
Strategy Interface
Context
Signal
Manager
```

---

## Phase 6：Risk

```text
Limits
Exposure
Kill Switch
Risk Manager
```

---

## Phase 7：Execution

```text
Trading Order
Order Manager
Backtest Execution
Paper Execution
Live Execution Interface
```

---

## Phase 8：Portfolio

```text
Position
PnL
Portfolio
Account
```

---

## Phase 9：Runtime

```text
完整闭环
```

---

## Phase 10：Python Bridge

```text
C++ API
Python SDK
```

---

## Phase 11：Web UI

```text
FastAPI
WebSocket
Frontend
DOM
Monitoring
```

---

## Phase 12：Windows 高强度测试

```text
Full Day
Multi Day
Stress
Repeat
Benchmark
```

---

## Phase 13：Linux 移植

```text
Build
Tests
Sanitizers
Performance
Deployment
```

---

## Phase 14：Paper

长时间运行。

---

## Phase 15：Live Qualification

满足全部实盘门槛后再上线。

---

# 51. 禁止事项

严禁：

```text
未经读取当前接口直接写代码

未经读取当前文件直接修改代码

只给代码片段

跳过测试

用 Python 替代 C++ 核心状态

Web UI 直接控制内部对象

OrderBook 未验证就开发策略

BACKTEST / PAPER / LIVE 三套重复逻辑

为了性能提前破坏架构

未经用户确认删除文件

未经核对 GitHub 创建同名重复模块

把交易订单与 L3 Market Order 混为一个类型

让 UI 阻塞交易核心

让 Python 成为实盘交易单点故障
```

---

# 52. 每次代码开发前固定流程

必须执行：

```text
1. 查询 GitHub / 当前源码

2. 找到对应 .hpp / .cpp / tests

3. 分析真实接口

4. 分析调用关系

5. 明确本次修改文件

6. 明确不修改文件

7. 用户确认需要时再修改

8. 输出完整文件

9. 编译

10. 跑测试

11. 回归测试

12. 报告结果
```

---

# 53. 每次修改后的固定报告

格式：

```text
本次修改：

1. xxx.hpp
2. xxx.cpp
3. test_xxx.cpp


未修改：

1. orderbook/*
2. risk/*
...


接口变化：

...


测试：

Unit:
PASS

Contract:
PASS

Integration:
PASS


Baseline:

PASSED
```

---

# 54. 项目最终要求

最终系统必须达到：

```text
Architecture Complete
Interface Unified
Core C++
Python Decoupled
Web UI Decoupled
Deterministic Backtest
Reliable Paper
Safe Live
Validated L3 OrderBook
Reproducible Results
Cross Platform
High Performance
Observable
Recoverable
Maintainable
Testable
```

最终架构：

```text
                     Databento CME L3
                            │
                            ▼
                    C++ Market Data
                            │
                            ▼
                     Replay / Live
                            │
                            ▼
                      L3 OrderBook
                            │
                            ▼
                     Feature Engine
                            │
                            ▼
                    Strategy Engine
                            │
                            ▼
                      Risk Manager
                            │
                            ▼
                     Order Manager
                            │
                            ▼
                    Execution Engine
                            │
            ┌───────────────┼───────────────┐
            ▼               ▼               ▼
        Backtest           Paper            Live
            │               │               │
            └───────────────┼───────────────┘
                            ▼
                           Fill
                            │
                            ▼
                    Portfolio / PnL
                            │
                            ▼
                    Metrics / State
                            │
                            ▼
                      Unified API
                            │
                            ▼
                      Python Bridge
                            │
                            ▼
                   FastAPI / WebSocket
                            │
                            ▼
                         Web UI
```

---

# 55. 项目最高优先级原则

```text
1. 正确性
2. 数据完整性
3. OrderBook 可靠性
4. 风险安全
5. 可测试性
6. 架构一致性
7. 可维护性
8. 性能
9. UI
```

任何优化都不能破坏前面的优先级。

---

# 56. 最终开发纪律

后续 ChatGPT 或开发人员接手本项目时必须遵守：

> **修改、添加、创建任何代码之前，必须先查询并确认对应的真实接口、源码、调用关系和测试。**

> **每次修改代码必须提供完整文件，不允许提供代码片段。**

> **已经通过高强度测试的模块默认冻结，未经真实 Bug 证据不得随意修改。**

> **C++ 是交易核心的唯一 Source of Truth；Python 与 Web UI 只能通过定义好的统一接口与核心交互。**

> **Windows + Visual Studio 2026 阶段必须达到完整稳定基线后，才能进入 Linux 企业级迁移。**

> **L3 OrderBook 必须经过多次、长时间、高强度、可重复的 Ground Truth 验证后，才允许作为策略与实盘基础。**


---

# ESU6 L3 高频剥头皮完整策略

## 一、完整策略总架构

```text
ESU6 Databento MBO L3
        ↓
OrderBook Reconstruction
        ↓
Feature Engine
        ↓
Regime Classifier
        ↓
┌──────────────────────────────┐
│ 1. Absorption                │
│ 2. Mean Reversion            │
│ 3. Liquidity Vacuum          │
│ 4. Momentum Breakout         │
│ 5. OBI Scalping              │
│ 6. Queue Position            │
│ 7. Market Making             │
└──────────────────────────────┘
        ↓
Composite Signal
        ↓
Risk Manager
        ↓
Execution Decision
        ↓
Order
        ↓
Fill
        ↓
Position / PnL
```

## 二、Market Regime 市场状态识别

至少区分：

```text
RANGE
TREND_UP
TREND_DOWN
BREAKOUT
HIGH_VOLATILITY
LOW_LIQUIDITY
NORMAL
```

| Regime | 主要策略 |
|---|---|
| RANGE | Mean Reversion / Absorption |
| TREND_UP | Momentum / OBI Long |
| TREND_DOWN | Momentum / OBI Short |
| BREAKOUT | Liquidity Vacuum / Momentum |
| HIGH_VOLATILITY | 降低仓位或停止 |
| LOW_LIQUIDITY | 禁止交易 |
| NORMAL | 多策略组合 |

核心原则：

```text
先判断“现在是什么市场”
再决定“使用什么策略”
```

## 三、L3 Feature Engine

### BBO / Price

```text
best_bid
best_ask
spread
mid_price
micro_price
last_trade_price
```

### Depth

```text
bid_depth_1
ask_depth_1
bid_depth_5
ask_depth_5
bid_depth_10
ask_depth_10
depth_imbalance
```

### Order Book Imbalance

```text
OBI_1
OBI_3
OBI_5
OBI_10
```

### Trade Flow

```text
aggressive_buy_volume
aggressive_sell_volume
trade_delta
buy_trade_count
sell_trade_count
large_trade_volume
```

### Order Flow

```text
add_bid
add_ask
cancel_bid
cancel_ask
modify_bid
modify_ask
cancel_rate
add_rate
OFI
```

### Queue / Refill

```text
queue_depletion
queue_refill
refill_rate
bid_queue_change
ask_queue_change
```

### Market Response

```text
price_response_to_buy_flow
price_response_to_sell_flow
flow_to_price_ratio
```

这是区分 Momentum 和 Absorption 的关键。

## 四、Absorption Strategy

做多：

```text
Aggressive Sell Volume ↑
+
Bid 被持续打
+
Bid Queue 持续 refill
+
价格没有继续下跌
+
MicroPrice 开始上移
→ LONG
```

做空：

```text
Aggressive Buy Volume ↑
+
Ask 持续被打
+
Ask Queue refill
+
价格不继续上涨
→ SHORT
```

确认条件：

```text
trade_flow
+
refill
+
price_response
+
spread
```

## 五、Mean Reversion Strategy

主要运行在 RANGE。

做多示例：

```text
Price << Fair Value
+
Sell Flow 衰竭
+
Bid Liquidity 恢复
+
OBI 转正
→ LONG
```

做空反之。

退出目标：

```text
Fair Value
MicroPrice
Mid
```

## 六、Liquidity Vacuum Strategy

上涨真空：

```text
Ask Depth 快速下降
+
大量 Ask Cancel
+
卖盘补充速度低
+
Aggressive Buy 持续
→ LONG
```

核心变量：

```text
depth_drop_rate
cancel_rate
refill_rate
aggressive_flow
spread
```

## 七、Momentum Breakout Strategy

条件：

```text
Trade Delta 强
+
OFI 强
+
MicroPrice 同方向
+
OBI 同方向
+
价格连续接受新价位
```

例如：

```text
Buy Flow ↑
Ask 被真正消费
Best Ask 上移
Bid 跟随上移
→ LONG
```

过滤：

```text
大量 Buy Flow
但价格不涨
```

可能是 Sell Absorption，不是 Momentum。

## 八、OBI Scalping

OBI 更适合作为方向确认器，而不是裸策略。

例如：

```text
OBI > threshold
+
MicroPrice > Mid
+
Buy Flow > Sell Flow
+
Ask Liquidity 下降
→ LONG
```

## 九、Queue Position Strategy

用途：

1. 预测价格方向。
2. 优化限价单执行与 Fill Probability。

关注：

```text
depletion
cancel
refill
queue_ahead
fill_probability
```

## 十、Market Making Strategy

成熟阶段再加入。

必须考虑：

```text
Spread
Inventory
Adverse Selection
Queue Position
Volatility
Toxic Flow
```

并支持：

```text
撤单
偏移报价
降低库存
```

## 十一、Composite Signal

每个策略统一输出：

```text
side
score
confidence
category
reason
```

示例：

```text
Absorption:
LONG
score = 0.80

OBI:
LONG
score = 0.60

Mean Reversion:
LONG
score = 0.70
```

然后计算 Composite Score。

RANGE 权重示例：

```text
Mean Reversion      0.40
Absorption          0.35
OBI                 0.15
Liquidity Vacuum    0.10
```

Breakout 权重示例：

```text
Momentum            0.40
Liquidity Vacuum    0.35
OBI                 0.20
Absorption          0.05
```

## 十二、Entry Filter

即使 Strategy 有 Signal，也必须过滤：

```text
Spread
Liquidity
Volatility
Time
Existing Position
Cooldown
Recent Losses
Data Integrity
Execution State
```

例如：

```text
spread > 2 ticks → NO TRADE
低流动性 → NO TRADE
数据异常 → NO TRADE
```

## 十三、Risk Management

### Trade Risk

```text
Max Order Size
Max Position
Stop Loss
Max Slippage
```

### Strategy Risk

```text
Max Consecutive Loss
Strategy Daily Loss
Strategy Disable
Cooldown
```

### Account Risk

```text
Daily Loss Limit
Max Drawdown
Exposure Limit
Margin
Buying Power
```

### System Risk

```text
Data Integrity
Latency
Execution Failure
Disconnected Feed
Broker Failure
Kill Switch
```

## 十四、Position Sizing

第一阶段：

```text
1 contract
```

之后根据：

```text
confidence
volatility
drawdown
```

动态调整，但始终受 Max Position 控制。

## 十五、Stop Loss

三类：

### Hard Stop

```text
N ticks
```

### Microstructure Stop

例如 Long Absorption：

```text
Bid 被击穿
+
Bid liquidity 消失
→ EXIT
```

### Time Stop

```text
若干秒 / 若干 F_LAST 后没有产生预期收益
→ EXIT
```

## 十六、Take Profit

三类：

```text
Fixed Tick Target
Fair Value Target
Microstructure Exit
```

例如 Momentum Long：

```text
Buy Flow 衰竭
Ask refill 大增
MicroPrice 回落
→ 提前退出
```

## 十七、Trade Management

持仓期间持续检查 Alpha：

```text
Entry
 ↓
每个稳定 F_LAST
 ↓
重新计算 Features
 ↓
Signal Strength
```

例如：

```text
0.85 → 0.30
```

则提前退出。

方向反转：

```text
立即退出
```

是否反手交给新的 Signal 决定。

## 十八、Session Filter

至少区分：

```text
Globex Overnight
Pre-market
US Open
Morning
Lunch
Afternoon
Close
```

不同 Session 可使用不同：

```text
阈值
仓位
策略权重
```

## 十九、完整策略最终形态

```text
                        ESU6 MBO L3
                             │
                             ▼
                    OrderBook Builder
                             │
                             ▼
                     Feature Engine
                             │
       ┌─────────────────────┼─────────────────────┐
       │                     │                     │
       ▼                     ▼                     ▼
 OrderBook Flow        Trade Flow             Regime
       │                     │                     │
       └─────────────────────┼─────────────────────┘
                             ▼
                     Strategy Manager
                             │
       ┌────────────┬────────┼────────┬─────────────┐
       ▼            ▼        ▼        ▼             ▼
 Absorption       Mean     Vacuum   Momentum       OBI
                Reversion
       │            │        │        │             │
       └────────────┴────────┼────────┴─────────────┘
                             ▼
                     Composite Signal
                             │
                             ▼
                       Entry Filters
                             │
                             ▼
                        Risk Manager
                             │
                             ▼
                    Position Sizing
                             │
                             ▼
                     Execution Engine
                             │
                             ▼
                           Fill
                             │
                             ▼
                        Portfolio
                             │
                             ▼
                    Trade Management
                             │
               ┌─────────────┼─────────────┐
               ▼             ▼             ▼
           Stop Loss     Take Profit    Time Exit
```

## 二十、当前核心交易组合

```text
Absorption
+
Mean Reversion
+
Liquidity Vacuum
+
Momentum Breakout
+
OBI Confirmation
+
Regime Filter
+
Composite Signal
+
Risk Management
+
Microstructure Exit
```

Queue Position 主要服务于后续执行优化。

Market Making 放到系统成熟以后再加入。
