# terminal 中文文档

[![原项目](https://img.shields.io/badge/原项目-microsoft--terminal-blue?style=flat-square&logo=github)](https://github.com/microsoft/terminal)
[![微信联系](https://img.shields.io/badge/微信-uaycar-brightgreen?style=flat-square&logo=wechat)](#)

> 本文档是 [microsoft/terminal](https://github.com/microsoft/terminal) 官方 README 的中文翻译版本,如有出入请以原项目英文文档为准。

## 项目简介

本仓库包含以下项目的源代码:

* [Windows Terminal](https://aka.ms/terminal)(新版 Windows 终端)
* [Windows Terminal Preview](https://aka.ms/terminal-preview)(预览版)
* Windows 控制台宿主 `conhost.exe`,以及两个项目共享的组件
* [ColorTool](https://github.com/microsoft/terminal/tree/main/src/tools/ColorTool) 配色工具与调用控制台 API 的[示例项目](https://github.com/microsoft/terminal/tree/main/samples)

相关仓库:[Windows Terminal 官方文档](https://learn.microsoft.com/windows/terminal)、[控制台 API 文档](https://github.com/MicrosoftDocs/Console-Docs)、[Cascadia Code 字体](https://github.com/Microsoft/Cascadia-Code)。

## 安装与运行 Windows Terminal

> [!NOTE]
> Windows Terminal 要求 Windows 10 2004(build 19041)或更高版本。

### Microsoft Store(推荐)

从 [Microsoft Store](https://aka.ms/terminal) 安装,新版本发布时自动升级,官方最推荐。

### 通过 GitHub 手动安装

从 [Releases 页面](https://github.com/microsoft/terminal/releases)的 **Assets** 区域下载 `Microsoft.WindowsTerminal_<versionNumber>.msixbundle`,双击即可安装;失败时可在 PowerShell 中执行:

```powershell
# 注意:PowerShell 7+ 请先执行
# Import-Module Appx -UseWindowsPowerShell

Add-AppxPackage Microsoft.WindowsTerminal_<versionNumber>.msixbundle
```

> [!NOTE]
> 手动安装可能需要 [VC++ v14 桌面框架包](https://learn.microsoft.com/troubleshoot/cpp/c-runtime-packages-desktop-bridge#how-to-install-and-update-desktop-framework-packages)(仅较旧 Windows 10 且报缺框架包错误时);手动安装不会自动更新,需定期更新以获取修复与改进。

### 通过 winget(Windows 包管理器 CLI)

```powershell
winget install --id Microsoft.WindowsTerminal -e
```

> 需要 [1.6.2631+](https://github.com/microsoft/winget-cli/releases) 的 WinGet 客户端以支持依赖项。

### 通过 Chocolatey / Scoop(非官方)

Chocolatey 包名为 `microsoft-windows-terminal`(`choco install` / `choco upgrade`),Scoop 在 extras bucket 中包名为 `windows-terminal`(`scoop bucket add extras` 后 `scoop install windows-terminal`)。

## 安装 Windows Terminal Canary

Canary 是 `main` 分支的每夜构建,可抢先体验尚未进入 Preview 的新特性,也是最不稳定的渠道。App Installer 版支持自动更新、仅限 Windows 11:https://aka.ms/terminal-canary-installer ;Portable ZIP 便携版不自动更新、支持 Windows 10(19041+):x64 https://aka.ms/terminal-canary-zip-x64 、ARM64 https://aka.ms/terminal-canary-zip-arm64 、x86 https://aka.ms/terminal-canary-zip-x86 。

## 终端与控制台概述

### Windows Terminal

Windows Terminal 是一款面向命令行用户的全新、现代、功能丰富且高效的终端应用,涵盖社区呼声最高的诸多特性:标签页、富文本、全球化、可配置性、主题与样式等,同时保持快速高效,不占用大量内存与电力。

### Windows 控制台宿主(conhost.exe)

`conhost.exe` 是 Windows 最原始的命令行体验,承载着控制台 API 服务器、输入引擎、渲染引擎、用户偏好等命令行基础设施。本仓库中的宿主代码正是系统内 `conhost.exe` 的真实构建源码。自 2014 年接手以来,团队为其新增了背景透明、按行选择、[ANSI / 虚拟终端序列](https://en.wikipedia.org/wiki/ANSI_escape_code)、[24 位色](https://devblogs.microsoft.com/commandline/24-bit-color-in-the-windows-console/)、[伪控制台 ConPTY](https://devblogs.microsoft.com/commandline/windows-command-line-introducing-the-windows-pseudo-console-conpty/) 等特性;但控制台必须向后兼容,无法加入标签页、Unicode、Emoji 等社区期待已久的功能——这正是新终端诞生的原因。

### 共享组件与新终端的诞生

改造控制台时,团队现代化了代码库:拆分模块与类、引入扩展点、用更安全的 STL 容器替换自制容器、借助 [WIL](https://github.com/Microsoft/wil) 库简化代码,产出可供任意 Windows 终端复用的关键组件:DirectWrite 渲染引擎、UTF-16/UTF-8 文本缓冲区、VT 解析器等。规划新终端时,团队决定继续深耕 C++ 代码库以复用这些组件,并把终端核心做成可嵌入其他应用的可复用 UI 控件——这就是今天从 Microsoft Store 或 [Releases](https://github.com/microsoft/terminal/releases) 下载到的 Windows Terminal。

## 常见问题

**自己构建并运行了新终端,但看起来和旧控制台一样?**

原因:在 Visual Studio 中启动了错误的项目。解决:请确保构建并部署的是 `CascadiaPackage` 项目。注意:`OpenConsole.exe` 只是本地构建的 `conhost.exe`,终端通过 ConPTY 借助它连接命令行应用。

## 文档与参与贡献

全部项目文档位于 [aka.ms/terminal-docs](https://aka.ms/terminal-docs),欢迎向[文档仓库](https://github.com/MicrosoftDocs/terminal)提交 Pull Request。动手写代码前,请先阅读[贡献者指南 CONTRIBUTING.md](https://github.com/microsoft/terminal/blob/main/CONTRIBUTING.md),避免重复劳动。

**新建 Issue 前请先搜索是否已有类似问题**;不适合开 Issue 的疑问可通过原 README 中列出的团队成员社交账号联系。

## 开发者指南

### 环境要求

克隆仓库后,推荐用 WinGet 配置文件一键配置环境(默认安装 Visual Studio 2026 Community 及所需工具,`.config` 目录下另有 Enterprise / Professional 版变体):

```powershell
winget configure .config\configuration.winget
```

手动配置要求:Windows 10 2004(build ≥ 10.0.19041.0)+;在设置中[开启开发者模式](https://learn.microsoft.com/windows/uwp/get-started/enable-your-device-for-development);[PowerShell 7+](https://github.com/PowerShell/PowerShell/releases/latest);版本 ≥ 10.0.26100.8249 的 [Windows 11 SDK](https://developer.microsoft.com/windows/downloads/windows-sdk/);至少 [VS 2026](https://visualstudio.microsoft.com/downloads/) 18.6,并安装工作负载「使用 C++ 的桌面开发」与「WinUI 应用程序开发」;构建测试项目还需 [.NET Framework 4.7.2 Targeting Pack](https://learn.microsoft.com/dotnet/framework/install/guide-for-developers#to-install-the-net-framework-developer-pack-or-targeting-pack)。

### 构建代码

用 Visual Studio 打开 `OpenConsole.slnx` 构建,或使用 **/tools** 目录下的便捷脚本。

PowerShell 方式:

```powershell
Import-Module .\tools\OpenConsole.psm1
Set-MsBuildDevEnvironment
Invoke-OpenConsoleBuild
```

Cmd 方式:

```shell
.\tools\razzle.cmd
bcz
```

### 运行与调试

右键 `CascadiaPackage` → 属性 → 调试,将「应用程序进程」与「后台任务进程」改为 "Native Only",按 F5 构建调试;平台选 "x64" 或 "x86"(C++ 应用,不支持 "Any CPU")。注意:直接运行 WindowsTerminal.exe 无法启动终端,原因见 Issue [#926](https://github.com/microsoft/terminal/issues/926) 与 [#4043](https://github.com/microsoft/terminal/issues/4043)。

### 编码规范

编码实践文档位于仓库 /doc 目录:[代码风格 STYLE.md](https://github.com/microsoft/terminal/blob/main/doc/STYLE.md)、[代码组织 ORGANIZATION.md](https://github.com/microsoft/terminal/blob/main/doc/ORGANIZATION.md)、[遗留代码异常处理 EXCEPTIONS.md](https://github.com/microsoft/terminal/blob/main/doc/EXCEPTIONS.md)、[WIL 智能指针与宏 WIL.md](https://github.com/microsoft/terminal/blob/main/doc/WIL.md)。

## 行为准则

本项目采用[微软开源行为准则](https://opensource.microsoft.com/codeofconduct/),详见[行为准则 FAQ](https://opensource.microsoft.com/codeofconduct/faq/),或邮件联系 opencode@microsoft.com。

---

**代部署 / 定制服务 / 技术咨询 请添加微信:uaycar**

本项目为 [microsoft/terminal](https://github.com/microsoft/terminal) 的中文翻译版本,所有代码版权归原项目作者所有,遵循其原始许可证(MIT License)。

**如果觉得有用,请给原项目点个 Star!** ⭐
