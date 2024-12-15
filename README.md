# ados

## Brief

- Lightweight runtime framework for Autonomous Driving or robotics. Based on modern c++ and just for Linux.


## Memo
- Runtime是可以作为一个自闭环的进程，包含所有的通用配置项（日志、执行器、分配器等）以及该进程对应的Modules（模块可以有多个）
- Module的配置包括输入、输出的channel，以及channel对应的backend具体实现
- 可以有多个进程的配置，也就是多个Runtime，编译完之后就是多个进程，外围需要有个脚本串起来这些进程
- 进程管理（保活、心跳、监控、数据）还需有一个或几个单独的进程，这些进程也是复用Runtime这套逻辑