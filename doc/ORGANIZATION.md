> 🌐 本文档由 [microsoft/terminal](https://github.com/microsoft/terminal) 翻译,英文原版见原项目。

> 注:原文超过 10000 字符,本文翻译覆盖核心章节,目录名与文件名保持原样。

# 代码组织

## 规则

- **遵循你在代码中已经看到的模式**
- 尽量把新想法/组件打包成接口定义良好的库
- 在扩展时把新想法封装成类,或把现有实现重构为类
- 每个项目应在其子目录的 ut_ 文件夹中有对应的单元测试(如 `ut_host`)
- 功能测试应放在 ft_ 子目录中(如 `ft_api`)
- 构建脚本通常放在以其输出类型命名的子目录中(如 `/dll` 或 `/exe`)
- 尽量把接口放在合适位置的 `inc` 文件夹中
- 把相关的库组织在一起(`/terminal/parser` 与 `/terminal/adapter`)

## 代码总览
* `/` - 根目录存放解决方案文件、根级 MD 文档和 SD 复制产物。
* `/bin` – 不入库,MSBuild 系统生成的二进制文件会放在这里
* `/dep` – 不属于 SDK 的依赖
	* `/dep/console` – 目前位于仅限内部的控制台私有 SDK 中的文件,我们正在推动开放
	* `/dep/DDK` – 从公开 Microsoft DDK 整体抽取的文件,免得你必须安装 DDK。我们正在减少对它们的依赖,但仍使用其中的 TAEF 作为测试运行引擎
	* `/dep/NT` – 来自 DDK 的更多结构,有些仅限内部/未公开,我们正试图移除
	* `/dep/telemetry` – Microsoft 私有遥测头文件
	* `/dep/wil` – Windows Internal Library – 与 Win32/NT/COM API 交互极为有用。为各种 Win32 API 提供大量"unique pointer"式的语法,以及一些帮助写出更干净代码的实用宏(RETURN_HR_IF、LOG_IF_WIN32_ERROR 等)
	* `/dep/win32k` – 来自 Windows 窗口系统的私有头文件,我们正试图迁移掉
* `/ipch` – 不入库,如果你用 Visual Studio 2015,IntelliSense 数据会生成在这里
* `/obj` – 不入库,MSBuild 系统生成的对象文件会放在这里
* `/src` – 重头戏。根目录是公共构建系统数据。
	* `/src/cascadia` - 该目录包含 Windows Terminal 专属的全部代码
		* `/src/cascadia/TerminalConnection` - 该 DLL 负责终端实例与不同终端后端通信的各种方式。例如 `ConptyConnection`(与 Windows 控制台进程通信)或与 Azure 通信的 `AzureCloudShellConnection`。
		* `/src/cascadia/TerminalSettings` - 该 DLL 负责为 TerminalCore 和 TerminalControl 抽象设置。它为 TerminalControl 的使用方提供一个向 Terminal 提供设置的公共接口。
		* `/src/cascadia/TerminalCore` - 该 LIB 负责终端实例的核心实现。它定义了一个重要的类 `Terminal`,即一个完整的终端实例,包含缓冲区、颜色表、VT 解析、输入处理等。它_不_规定任何 UI 实现——它应连接到能渲染其内容并向其提供输入的代码。
		* `/src/cascadia/TerminalControl` - 该 DLL 提供 `TermControl` 的 UWP-XAML 实现,可嵌入应用程序中为应用提供终端实例。它包含一个用于在屏幕上绘制文本的 DX 渲染器,并把输入转换后发送给核心 Terminal。它还接收应用于自身和核心 Terminal 的设置。
		* `/src/cascadia/TerminalApp` - 该 DLL 代表 Windows Terminal 应用的实现,包括解析设置、承载带终端的标签页和窗格、显示其他 UI 元素。该 DLL 几乎完全是 UWP 风格代码,不应做任何 Win32 风格的 UI 工作。
		* `/src/cascadia/WindowsTerminal` - 该 EXE 为 TerminalApp 提供 Win32 宿主。它负责建立 XAML Islands,并负责绘制窗口——既可以是标准窗口,也可以把内容画进标题栏(非客户区)。
		* `/src/cascadia/CascadiaPackage` - 用于把 Windows Terminal 及其依赖打包成 .appx/.msix 以部署到机器上的项目。
		* `/src/cascadia/WpfTerminalControl` - WPF 版终端控件的 DLL 实现。
	* `/src/host` – Windows 控制台宿主的核心。包括缓冲区、输入、输出、窗口、服务器管理、剪贴板,以及其他文档未提及的大多数与控制台宿主窗口的交互。我们正努力把可复用的部分抽到其他库,但这仍在进行中
		* `/src/host/lib` – 构建宿主的可复用 LIB 副本
		* `/src/host/dll` – 把 LIB 打包成 conhostv2.dll,放入系统的 C:\windows\system32\
		* `/src/host/exe` – 把 LIB 打包成 OpenConsole.exe,目前用于在不替换系统 system32 副本的情况下测试
		* `/src/host/tools` – 让测试/调试/开发更轻松的零碎工具
			* ...待补充文档,逐一说明它们是什么
		* `/src/host/ut_host` – 包含我们成功覆盖了单元测试的所有内容的完整单元测试库。希望所有新代码都在这里贡献适当的单元测试
		* `/src/host/ft_api` – 针对任何改变我们通过 API 与外界交互方式的内容的功能级测试。我们也在工作中持续补充
		* `/src/host/ft_cjk` – 针对中日韩双宽/双字节特性的专项测试,以前必须在另一环境中运行。终有一天会并入 ft_api
		* `/src/host/ft_resize` – 针对缓冲区窗口调整大小/重排的专项测试
		* `/src/host/ft_uia` – 目前已禁用(因为不太可靠)的 UI 自动化测试,我们希望重新启用并扩展,以覆盖各类人工交互的 UI 自动化
		* `/src/host/...` - 我下面列出的文件
	* `/src/inc` – 宿主与其他一些库共享的头文件。这里只是其中一部分。目前的头文件组织有点乱,但我们希望将来清理
	* `/src/propslib` – 控制台宿主与 OS 外壳"右键快捷方式文件并修改控制台属性"页面共享的库,用于在注册表和快捷方式 LNK 内嵌数据中读写用户设置
	* `/src/renderer` – 从原代码中重构抽取出的、与"把缓冲区中的文本渲染到屏幕"相关的全部活动
		* `/src/renderer/base` – 基础接口层,提供与引擎无关的渲染功能,如从控制台缓冲区选择数据、决定如何布局/变换数据,然后把命令分派给特定的最终显示引擎
		* `/src/renderer/gdi` – 面向屏幕渲染的 GDI 实现。把来自 base 层的"画一条线""填充背景""选择区域"等命令转换为对屏幕的 GDI 调用。从原始控制台宿主代码中抽取而来
		* `/src/renderer/inc` – 所有渲染器通信的接口定义
	* `/src/terminal` – 控制台的虚拟终端支持。指与 STDIN/STDOUT 上其他文本一起带内出现的、命令显示器执行动作的序列。这是 \*nix 控制终端的方式
		* `/src/terminal/parser` – 包含一个状态机和分拣引擎,把 STDOUT 或 STDIN 传入的单个字符解码成应当执行的相应动词
		* `/src/terminal/adapter` – 把来自接口的动词转换为对控制台 API 的调用。它并不真正调用 API(出于性能考虑,因为它与调用方在同一个二进制内),但尽量保持与 API 调用一致。API 存在一些私有扩展,用于编写这段代码时尚不存在的行为,我们尚未公开它们。还没决定是否会公开,还是强制大家用 VT 来访问
	* `/src/tsf` – Text Services Foundation(文本服务框架)。为控制台提供 IME 输入服务。历史上只用于以中文、日文、韩文为主语言安装的操作系统上的对应 IME。2016 年夏天解除了限制,可以在任何系统安装上配合任何 IME 使用(显示是否正确是另一回事)。同时也解除限制,让笔和触摸输入(经由 IME 消息路由)能从 TabTip 窗口(帮助你向应用插入手写/触摸/键盘候选词的小弹窗)在控制台内正确显示

## 宿主文件总览

* 一般与处理输入/输出数据相关,有时与实际服务调用交织
	* `_output.cpp`
	* `_stream.cpp`
* 处理复制/粘贴等
	* `clipboard.cpp`
* 处理你在 CMD.exe 中看到的命令提示行(即"已处理输入行"…… 大多数其他 shell 用原始输入自行处理,不用我们的。这是糟糕架构设计的遗留——把东西放进了 conhost 而不是 CMD)
	* `cmdline.cpp`
* 包含整个控制台应用的全局状态
	* `consoleInformation.cpp`
* 与通过我们的协议同驱动进行底层服务器通信相关的内容
	* `Csrutil.cpp`
	* `Srvinit.cpp`
	* `Handle.cpp`
* 与应用启动相关的例程
	* `Srvinit.cpp`
* 与 API 调用相关的例程(及其服务化,与服务器协议略有交织)
	* `Directio.cpp`
	* `Getset.cpp`
	* `Srvinit.cpp`
* 附加在缓冲区上以支持 CJK 语言的额外内容
	* `Dbcs.cpp`
* 将现有光标结构类化的尝试,使其脱离 Screen Info/Text Info
	* `Cursor.cpp`
* 与在控制台回滚缓冲区(screeninfo/textinfo 中定义的输出滚动缓冲区)中搜索相关
	* `Find.cpp`
* 包含全局状态数据
	* `Globals.cpp`
* 将现有图标操作类化的尝试,使其脱离 ConsoleInformation/Settings
	* `Icon.cpp`
* 包含全部键盘/鼠标输入处理、按键捕获、按键转换,以及对输入缓冲区的一些操作
	* `Input.cpp`
	* `Inputkeyinfo.cpp`
	* `Inputreadhandledata.cpp`
* 仅由 OS 用来向 conhostv2.dll 传递预配置驱动句柄的主入口
	* `Main.cpp`
* 各类实用程序与杂项
	* `Misc.cpp`(历代随机控制台开发者留给我们的)
	* `Util.cpp`(我们这个时代创建的)
* 自定义清零且不抛异常的分配器
	* `Newdelete.cpp`
* 与向 TextInfo 缓冲区插入文本相关
	* `Output.cpp`
	* `Stream.cpp`
* 连接 PropsLib 中的接口以操作持久化设置状态
	* `Registry.cpp`
* 连接我们较新抽取出的渲染器 LIB,向其提供控制台状态和用户偏好数据
	* `renderData.cpp`
	* `renderFontDefaults.cpp`
* 维护关于窗口内应呈现内容的大部分信息(尺寸、维度,还持有一个文本缓冲区实例、一个光标实例和一个选择实例)
	* `screenInfo.cpp`
* 处理鼠标与键盘滚动的某些方面
	* `Scrolling.cpp`
* 处理屏幕上点击拖拽高亮选择文本(以及可进入模式后用键盘环绕选择的 Mark 模式选择)。完成后常调用剪贴板
	* `Selection.cpp`
	* `selectionInput.cpp`
	* `selectionState.cpp`
* 处理所有用户偏好和状态。从 consoleInformation 中抽取而来,CI 仍继承它(因为这种关联很难拆开)
	* `Settings.cpp`
* 好老 Windows 10 遥测管线和 ETW 事件(调试辅助,它们使用同一通道、不同标志)
	* `Telemetry.cpp`
	* `Tracing.cpp`
* 私有调用 Windows 窗口管理器以执行与控制台进程相关的特权操作(正努力消除)或 High DPI 相关操作(也在努力消除)
	* `Userprivapi.cpp`
	* `Windowdpiapi.cpp`
* 窗口大小调整/布局/管理/窗口消息循环,以及其他所有让我们与 Windows 交互以创建可视显示面并控制用户交互入口的东西
	* `Window.cpp`
	* `Windowproc.cpp`
