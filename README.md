# uviot

## 目标
* 提供一个可以工作的IOT软件框架

## Feature
* 支持模块化开发，类似于Linux kernel 风格
* 支持插件功能
* 基于libuv库，支持跨平台开发

## 安装说明
### 下载
### 编译
### 安装

## 目录说明
* __adapters__ 用于存放设备的适配代码，类似于linux kernel中的driver
* __board__ 存放不同单板的配置或相关代码
* __core__ uviot核心代码，包括module机制、关键数据结构和代码等
* __include__ uviot暴露的借口文件
* __io__ 保存单板硬件接口访问接口
* __lib__ uviot引用的外部库或者对外提供的库
* __release__ 保存最终编译结果，make install?
* __services__ 一些高层服务
* __utils__ 一些辅助工具
