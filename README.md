# STM32F1-freeRTOS

> 基于stm32f103vet6最小核心板的freeRTOS练习，参考教程为正点原子相关教程
>
> freeRTOS Version: V9.0.0
>
> Keil MDK5.24

`stm32f103VET6内部存储：512k Flash(ROM),64k SRAM(内存)`

### 工程目录

1. LedTask:freeRTOS创建任务，动态创建，控制LED闪烁
2. LedTask-static:添加静态创建任务方法
3. suspend:挂起及恢复任务
4. interrupt:中断的开启与关闭对于定义的中断优先级的影响。例程设置为大于5的优先级中断受控。
5. list:列表
6. MsgQueue:消息队列
7. Semaphore,SemaphoreISR:二值信号量，计数信号量，中断与非中断使用
8. Mutex:互斥信号量







### 更新时间

- 2020.12.27：添加 1.LedTask
- 2020.12.27:  add 01.LedTask-static
- 2020.12.29：add 02.os-TaskSuspend
- 2021.01.03：add 02.os-interrupt
- 2021.01.10:   add 03~06

