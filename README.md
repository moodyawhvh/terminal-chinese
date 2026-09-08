<div align="center">

# terminal 中文翻译版

**[中文版] terminal — 全新 Windows 终端与经典 Windows 控制台宿主**

[![原项目](https://img.shields.io/badge/原项目-microsoft--terminal-blue?style=flat-square&logo=github)](https://github.com/microsoft/terminal)
[![中文文档](https://img.shields.io/badge/中文文档-README.zh--CN.md-orange?style=flat-square)](README.zh-CN.md)
[![GitHub Stars](https://img.shields.io/github/stars/microsoft/terminal?style=flat-square&label=原项目Stars)](https://github.com/microsoft/terminal/stargazers)
[![微信联系](https://img.shields.io/badge/微信-uaycar-brightgreen?style=flat-square&logo=wechat)](#)

</div>

---

> 这是 [microsoft/terminal](https://github.com/microsoft/terminal) 的中文翻译版本。
> 完整源代码请访问原项目:https://github.com/microsoft/terminal

**代部署 / 定制服务 / 技术咨询 请添加微信:uaycar**

---

## 📖 项目简介

Windows Terminal 是微软开源的新一代终端应用,为命令行用户带来标签页、富文本、全球化、高度可配置与主题样式等 Windows 命令行社区期待已久的现代体验;本仓库同时包含经典控制台宿主 `conhost.exe` 以及两个项目共享的核心组件源码。这里是该项目的中文翻译介绍版,帮助中文用户快速了解、安装与上手。

## ✨ 主要特性

- **多标签页与富文本**:单窗口管理多个命令行会话,支持拆分窗格
- **全球化与 Unicode**:完整 Unicode 文本、Emoji 与多语言显示
- **高度可配置**:Profile、快捷键、主题与样式均可自由定制
- **GPU 加速渲染**:基于 DirectWrite 的现代文本布局与渲染引擎
- **现代终端能力**:ANSI / 虚拟终端序列、24 位真彩色、ConPTY 伪控制台
- **经典控制台宿主**:包含 Windows 原生命令行体验 `conhost.exe` 的真实源码
- **共享组件可复用**:DirectWrite 渲染引擎、UTF-8/UTF-16 文本缓冲、VT 解析器等
- **多版本渠道**:稳定版、Preview 预览版、Canary 每夜构建任你选择

## 📁 文件说明

| 文件 | 说明 |
|:-----|:-----|
| README.md | 本文件(中文简介) |
| README.zh-CN.md | 详细中文文档(完整汉化) |

## 🚀 快速开始

1. 系统要求:Windows 10 2004(build 19041)或更高版本
2. 推荐从 Microsoft Store 安装(自动更新):https://aka.ms/terminal
3. 或使用 winget 安装:

```powershell
winget install --id Microsoft.WindowsTerminal -e
```

4. 或从 GitHub [Releases 页面](https://github.com/microsoft/terminal/releases)下载 `.msixbundle` 双击安装,失败时可用 PowerShell 执行 `Add-AppxPackage`
5. 社区包管理器同样可用:Chocolatey(`microsoft-windows-terminal`)、Scoop(`windows-terminal`)
6. 想抢先体验新特性,可安装 [Canary 每夜构建](https://aka.ms/terminal-canary-installer)
7. 安装完成后,在 CMD、PowerShell 或「运行」对话框中输入 `wt` 即可启动

完整源代码与最新版本请访问原项目:https://github.com/microsoft/terminal

## 📞 联系方式

**代部署 / 定制服务 / 技术咨询 请添加微信:uaycar**

---

本项目为 [microsoft/terminal](https://github.com/microsoft/terminal) 的中文翻译版本,所有代码版权归原项目作者所有,遵循其原始许可证(MIT License)。

**如果觉得有用,请给原项目点个 Star!** ⭐
