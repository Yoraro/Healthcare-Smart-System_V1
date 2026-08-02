# 基于 STM32 的康养智慧小屋监控系统

本项目以 STM32F103C8T6 为控制核心，实现康养小屋环境数据采集、本地显示、自动联动、异常报警、节能控制和 SD 卡日志记录。

## 功能概览

- 每 5 秒采集温度、湿度、CO2 和 PM2.5 数据。
- 根据温湿度、CO2 和 PM2.5 计算空气质量综合评分。
- 使用 HC-SR501 检测人体活动，并累计活动触发次数。
- OLED 显示时间、环境数据、工作模式、设备状态和 SD 卡状态。
- 自动模式下根据环境阈值控制风扇，根据时间和人体活动控制灯光与窗帘。
- 支持自动、手动和节能三种工作模式。
- CO2、PM2.5 或温度超过报警阈值时启动蜂鸣器。
- 连续 2 小时未检测到人体活动时进入节能模式。
- 每 10 秒将传感器和设备状态写入 SD 卡 CSV 文件。
- 可通过按键修改舒适阈值和报警阈值。

## 硬件组成

| 模块 | 用途 | 主要接口 |
| --- | --- | --- |
| STM32F103C8T6 | 主控制器 | - |
| DHT11 | 温湿度采集 | PA0 |
| JW01 | CO2 采集 | USART3，PB10/PB11 |
| DC01 | PM2.5 采集 | USART2，PA2/PA3 |
| HC-SR501 | 人体活动检测 | PB8 |
| OLED | 本地数据显示 | I2C1 |
| DS1302 | 实时时钟 | PA11/PA12/PA15 |
| 风扇 | 通风控制 | PB9 |
| LED | 灯光模拟 | 参见 `HARDWARE/LED` |
| 28BYJ-48 | 窗帘模拟 | PB0/PB1/PB3/PB4 |
| SD 卡 | CSV 日志保存 | SPI，CS 为 PB5 |

接线前应同时检查原理图和各驱动头文件中的引脚定义。

## 软件结构

```text
Core/       STM32CubeMX 生成的启动代码和主函数
CTRL/       系统初始化、模式切换、报警和联动控制
HARDWARE/   传感器、OLED、按键及执行器驱动
SAVE/       SD 卡、FatFs 和日志相关代码
SYSTEM/     延时、串口调试等基础支持代码
MDK-ARM/    Keil uVision 工程及构建输出
```

系统主流程位于 `CTRL/Src/sys_ctrl.c`。主循环持续调用 `SYS_CTRL_Task()`，函数内部使用 `HAL_GetTick()` 实现周期调度。

## 默认阈值

| 类型 | 默认值 |
| --- | --- |
| 舒适温度 | 15～28 摄氏度 |
| 舒适湿度 | 40%～60% |
| 自动通风 CO2 阈值 | 1000 ppm |
| 舒适 PM2.5 阈值 | 75 ug/m3 |
| 高温报警 | 大于 35 摄氏度 |
| 低温报警 | 小于 0 摄氏度 |
| CO2 报警 | 大于 1200 ppm |
| PM2.5 报警 | 大于 150 ug/m3 |
| 自动节能 | 连续无人 2 小时 |

阈值可通过 OLED 设置页面和按键修改，修改结果当前保存在 RAM 中，重新上电后恢复默认值。

## 编译方法

1. 安装 Keil MDK-ARM，并安装 STM32F1 Device Family Pack。
2. 打开 `MDK-ARM/STM32F1_KS.uvprojx`。
3. 选择 `STM32F1_KS` Target 后执行 Build。
4. 编译生成的 HEX 文件位于 `MDK-ARM/STM32F1_KS/STM32F1_KS.hex`。

工程当前使用 ARM Compiler 5，目标芯片为 STM32F103C8。

## SD 卡日志

日志按日期保存为 `YYYY-MM-DD.csv`，主要字段包括：

```text
Time,temp,humi,co2,pm25,air_score,human_detected,
activity_count,last_human_tick_ms,fan_on,led_on,curtain_open,
curtain_current,curtain_moving,alarm_active,eco_manual_set,
eco_enter_time,mode_changed
```

`activity_count` 表示人体红外传感器由无信号变为有信号的累计次数，不代表准确人数。

## 已知限制

- 单个 HC-SR501 只能检测人体活动，不能判断进出方向或准确统计屋内人数。若需要人员进出计数，应使用门口双传感器、毫米波雷达或视觉方案。
- 当前温湿度使用无符号整数保存，因此不能表示零下温度。若需要验证低于 0 摄氏度报警，应更换支持负温度的传感器，并将温度字段改为有符号类型。
- 阈值修改尚未写入 Flash 或 EEPROM，断电后不会保留。
  
## 说明

本项目的SYSTEM模块为移植正点原子相关代码，仅用于学习使用，另外使用需遵循正点原子相关要求。

## 许可证

本项目主要用于课程设计和学习交流。第三方 HAL、CMSIS 和 FatFs 代码遵循其各自许可证。
