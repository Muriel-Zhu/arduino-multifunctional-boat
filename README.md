# Arduino Multifunctional Catamaran Prototype

这是一个基于 Arduino 的多功能双体船原型项目，项目报告整理自课程/实践设计成果，适合作为工程项目作品集展示。

> Personal learning and portfolio use only. Not for commercial use.

## Project Overview

本项目围绕小型遥控水面载具设计展开，目标是构建一套具备稳定航行、基础载荷搭载和水面交互能力的双体船原型。项目采用双船体结构提升横向稳定性，并结合 Arduino 控制、电机驱动、舵机机构和 3D 打印结构件完成系统集成。

## Key Features

- 双体船结构设计，提高水面航行稳定性和载荷承载能力。
- 使用 SolidWorks 完成船体、甲板、舱体和模块化结构建模。
- 基于 Arduino 搭建控制系统，实现推进与机械交互控制。
- 使用电机驱动模块、舵机和电源模块完成硬件集成。
- 通过 Tinkercad 等工具进行电路逻辑验证，降低硬件调试风险。
- 采用 3D 打印完成结构件制作，并进行装配与迭代优化。

## My Contributions

- 参与项目方案设计、结构建模与原型迭代。
- 使用 SolidWorks 完成核心结构件建模，并考虑 3D 打印装配可行性。
- 参与 Arduino 控制逻辑设计与硬件连接调试。
- 完成项目报告整理，梳理项目背景、开发流程、机械结构、电子架构和测试过程。

## Repository Structure

```text
.
├── README.md
├── .gitignore
├── docs
│   └── project-report.pdf
├── models
│   └── stl
│       ├── cabin.stl
│       ├── cage.stl
│       ├── frame.stl
│       ├── head.stl
│       ├── main-deck.stl
│       ├── marine-propeller-left.stl
│       ├── marine-propeller-right.stl
│       ├── pontoon-left.stl
│       └── pontoon-right.stl
└── src
    └── arduino
        └── sketch_apr20a
            └── sketch_apr20a.ino
```

## Project Report

完整项目报告见：

[docs/project-report.pdf](docs/project-report.pdf)

## Source Code

Arduino 控制代码见：

[src/arduino/sketch_apr20a/sketch_apr20a.ino](src/arduino/sketch_apr20a/sketch_apr20a.ino)

代码实现了双模式控制逻辑，包括航行模式、起重机模式、摇杆输入、舵机控制、电机驱动、超声波测距和 OLED 状态显示。

## 3D Printable Models

STL 模型文件位于：

[models/stl](models/stl)

这些文件用于展示双体船结构件和 3D 打印装配部件，包括船体浮筒、主甲板、舱体、框架、螺旋桨和保护结构等。

## Tech Stack

- Arduino
- SolidWorks
- 3D Printing
- Tinkercad
- Motor Driver
- Servo Control
- Embedded System Prototyping

## Notes

本仓库用于个人学习、作品集展示和求职材料补充，不包含商业交付内容，不用于商业用途。
