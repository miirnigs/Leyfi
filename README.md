# Leyfi

Leyfi 是一个可进行二次开发的卡密系统，旨在为程序开发者制作的程序提供**安全**，**灵活**，**高效**的授权保护。
基于**MFC架构**和**IOCP通讯模型**的它具备简洁的界面与高效的性能优势。
Leyfi 为开发者提供一个平台，可以方便的管理正在使用程序的用户。
防止用户将程序转卖，或禁止某些用户使用程序，有效保护程序开发者的权益。

<!-- PROJECT SHIELDS -->
[![GitHub stars](https://img.shields.io/github/stars/miirnigs/Authorisation-System)](https://github.com/miirnigs/Authorisation-System/stargazers)
[![GitHub forks](https://img.shields.io/github/forks/miirnigs/Authorisation-System?color=%23FFFF00)](https://github.com/miirnigs/Authorisation-System/network)
[![GitHub license](https://img.shields.io/github/license/miirnigs/Authorisation-System?color=%23A52A2A)](https://github.com/miirnigs/Authorisation-System/blob/Main/LICENSE)
[![GitHub issues](https://img.shields.io/github/issues/miirnigs/Authorisation-System?color=%09%23FFA500)](https://github.com/miirnigs/Authorisation-System/issues)
![GitHub language](https://img.shields.io/badge/language-C%2B%2B-lightgrey)

## 项目结构
* ### 界面
  * [x] 服务端交互界面
  * [x] 客户端交互界面
* ### 通讯
  * [x] 服务端采用IOCP架构处理消息
  * [ ] 客户端TCP连接到服务端
* ### 数据安全
  * [ ] 数据包进行 **AES** 和 **RSA** 双重加密
  * [ ] 数据包末尾添加 **签名** 校验
  * [ ] 数据包添加服务端提供的唯一 **Token**
  * [ ] 根据时间使用 **雪花算法** 生成校验值
* ### 防破解
  * [ ] 可以使用 **远程数据**
  * [ ] 客户端交互界面进行防破解处理
  * [ ] 客户端添加 **暗桩** 防止被调试
  * [ ] 客户端的函数进行动态调用
 > 防破解思路不宜公开，此处也只是指出了部分思路，在代码里也不会有过多的防破解技巧。

## 软件截图
![image](https://github.com/miirnigs/Authorisation-System/blob/Main/img/ServerMain.png)
![image](https://github.com/miirnigs/Authorisation-System/blob/Main/img/ServerPort.png)
![image](https://github.com/miirnigs/Authorisation-System/blob/Main/img/ClientMain.png)

## 报告问题
* BUG或者使用上的问题可以提交到 [Leyfi/issues](https://github.com/miirnigs/Leyfi/issues)

## 许可证
该项目签署了Apache-2.0 授权许可，详情请参阅 [LICENSE](https://github.com/miirnigs/Authorisation-System/blob/Main/LICENSE)
