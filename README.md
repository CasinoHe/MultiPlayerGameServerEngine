# A simple general multiplayer game server

[中文简介](README.cn.md)

## build status

![ubuntu](https://github.com/casinohe/SimpleMultiPlayerGameServer/actions/workflows/ubuntu.yml/badge.svg)
![macos](https://github.com/casinohe/SimpleMultiPlayerGameServer/actions/workflows/macos.yml/badge.svg)
![windows](https://github.com/casinohe/SimpleMultiPlayerGameServer/actions/workflows/windows.yml/badge.svg)

## Introduction

The project has been written in spare time since early 2023, and the first stable version is expected to be released in May 2023.

There are not many open source and relatively complete server engines, and some of them are closely tied to game content.

It is hoped that through the author's more than 10 years of experience in game industry development, a relatively stable, efficient server engine that is not bound to game content can be completed.

At the same time, it can gradually replace the server engine of the author's project, saving costs in a more stable and efficient way.

## Expected Features

- Open source
- Stable, each tag has a large number of test cases, as well as pressure testing, performance testing, and the formation of test reports to ensure stability.
- Fat script, thin engine, the engine provides the most basic network and data interface, and the rest can be implemented in the script.
- Massive multiple player support. Supports more than 100,000 connections on single process at the same time.
- Efficiently multi-threaded processing compression and encryption, and guarantee the connection processing of huge amounts of players.
- The bottom layer is based on c++, and the script can use multiple scripts such as python and lua, and the scripts can coexist interactively, which can greatly improve the development efficiency

## Usage
