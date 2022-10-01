# MyReactor

## 项目介绍

* 个人C++服务器编程练习项目，模仿muduo实现的基于libev的网络编程库，与MyReactor类似，使用libev代替Poller和EPollPoller

## 开发环境

* linux kernel version 4.15.0 (ubuntu 20.04 desktop)
* gcc version 7.5.0
* cmake version 3.10.2

## 编译指导

* 先需要安装libev
```shell
sudo apt install libev-dev
```

* 开始编译
```shell
git clone ${project_url}
cd MyReactor
mkdir -p build
cd build
cmake ..
make
```
