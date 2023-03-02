# 一个简单的通用多人联网游戏服务器引擎

[Readme in English](README.md)

## build status

![ubuntu](https://github.com/casinohe/SimpleMultiPlayerGameServer/actions/workflows/ubuntu.yml/badge.svg)
![macos](https://github.com/casinohe/SimpleMultiPlayerGameServer/actions/workflows/macos.yml/badge.svg)
![windows](https://github.com/casinohe/SimpleMultiPlayerGameServer/actions/workflows/windows.yml/badge.svg)

## 简介

该项目是从2023年初开始利用业余时间编写，预期在2023年5月能够发布第一个稳定版本。
开源的比较完整的服务器引擎不多，不少和游戏内容绑定比较密切。
希望通过作者10多年的游戏行业开发经验，完成一个比较稳定，运行高效，不和游戏内容绑定的服务器引擎。
同时也能够逐步替换作者所在项目的服务器引擎，用更稳定、运行更高效的方式节省成本。

## 目标特性

- 开源
- 稳定，每个tag发布都有大量的测试用例，也有压测，性能测试，形成测试报告，保证稳定性。
- 肥脚本，瘦引擎，引擎提供最基本的网络、数据接口，其余都可以在脚本实现。
- 单进程支持超过10万的同时在线玩家连接
- 网络层高效实现多线程处理压缩、加密，为巨量玩家的连接处理做保证
- 底层基于c++，脚本可采取python，lua等多个脚本，并且脚本能够交互共存，能够极快的提升开发效率

## 使用
