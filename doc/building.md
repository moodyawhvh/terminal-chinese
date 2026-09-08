> 🌐 本文档由 [microsoft/terminal](https://github.com/microsoft/terminal) 翻译,英文原版见原项目。

> 注:原文超过 10000 字符,本文翻译覆盖核心章节,命令、脚本与日志保持原样。

# 如何构建 OpenConsole

本仓库的部分依赖使用 [git submodules](https://git-scm.com/book/en/v2/Git-Tools-Submodules)。为确保子模块被还原或更新,构建前务必运行:

```shell
git submodule update --init --recursive
```

OpenConsole.slnx 可以在 Visual Studio 内构建,也可以用 **/tools** 目录中的一组便捷脚本和工具在命令行构建:

使用 Visual Studio 时,请务必设置代码格式化路径。要下载所需的 clang-format.exe 文件,先按下面的构建说明操作一次,然后运行:
```powershell
Import-Module .\tools\OpenConsole.psm1
Set-MsBuildDevEnvironment
Get-Format
```
之后,在 Visual Studio 中转到 Tools > Options > Text Editor > C++ > Formatting,勾选 "Use custom clang-format.exe file",并通过勾选框下方的"浏览"按钮选择仓库中的 /packages/clang-format.win-x86.10.0.0/tools/clang-format.exe。

### 在 PowerShell 中构建

```powershell
Import-Module .\tools\OpenConsole.psm1
Set-MsBuildDevEnvironment
Invoke-OpenConsoleBuild
```

还有几个额外的导出函数(详见各自文档):

- `Invoke-OpenConsoleBuild` - 构建解决方案。可传入 msbuild 参数。
- `Invoke-OpenConsoleTests` - 运行各类测试。默认运行单元测试。
- `Start-OpenConsole` - 从输出目录启动 Openconsole.exe。默认运行 x64。
- `Debug-OpenConsole` - 启动 Openconsole.exe 并附加默认调试器。默认运行 x64。
- `Invoke-CodeFormat` - 使用 clang-format 将所有 C++ 文件格式化为我们要求的代码风格。

### 在 Cmd 中构建

```shell
.\tools\razzle.cmd
bcz
```

也有运行测试的脚本:
- `runut.cmd` - 运行单元测试
- `runft.cmd` - 运行功能测试
- `runuia.cmd` - 运行 UIA 测试
- `runformat` - 使用 clang-format 将所有 C++ 文件格式化为我们要求的代码风格

## 运行与调试

要在 VS 中调试 Windows Terminal,右键点击 `CascadiaPackage`(在解决方案资源管理器中)进入属性。在"调试"菜单中,把 "Application process" 和 "Background task process" 改为 "Native Only"。

然后你就可以按 <kbd>F5</kbd> 构建并调试 Terminal 项目了。

> 👉 你将_无法_直接运行 WindowsTerminal.exe 来启动 Terminal。原因详见 [#926](https://github.com/microsoft/terminal/issues/926)、[#4043](https://github.com/microsoft/terminal/issues/4043)

## 配置类型

Openconsole 有三种配置类型:

- Debug
- Release
- AuditMode

AuditMode 是一种实验性模式,会启用 CppCoreCheck 的额外静态分析。

## 更新 Nuget 包引用 - 全局统一版本
本项目的大多数 Nuget 包引用集中在一个配置里,使所有内容只有一个规范版本。这个规范版本会在构建前由构建管线、环境初始化脚本或 Visual Studio(视情况)还原。

规范版本号定义在 dep/nuget/packages.config 中,它决定了 nuget.exe 会下载什么。大多数 Nuget 包还带有 .props 和/或 .targets 文件,每个使用该包的项目都必须导入它们。这些导入语句统一放在:
- src/common.nugetversions.props
- src/common.nugetversions.targets

当全局管理的版本变化时,上述三个文件必须同步修改。

## 更新 Nuget 包引用 - 本地版本
本项目中的某些 Nuget 包引用(如 `Microsoft.UI.Xaml`)必须在 Visual Studio NuGet 包管理器之外更新。可以用下面的代码片段完成。
> 注意:运行该片段需要使用 WSL,因为命令用到了 `sed`。
更新某个包的版本,使用如下片段:

`git grep -z -l $PackageName | xargs -0 sed -i -e 's/$OldVersionNumber/$NewVersionNumber/g'`

其中:
- `$PackageName` 是包名,例如 Microsoft.UI.Xaml
- `$OldVersionNumber` 是当前使用的版本号,例如 2.4.0-prerelease.200506002
- `$NewVersionNumber` 是你要迁移到的版本号,例如 2.5.0-prerelease.200812002

用法示例:

`git grep -z -l Microsoft.UI.Xaml | xargs -0 sed -i -e 's/2.4.0-prerelease.200506002/2.5.0-prerelease.200812002/g'`

## 使用 .nupkg 文件代替下载的 Nuget 包
如果你想用 .nupkg 文件代替下载的 Nuget 包,可以按以下步骤操作:

1. 打开 Nuget.config,取消第 8 行("Static Package Dependencies")的注释
2. 创建文件夹 /dep/packages
3. 把你的 .nupkg 文件放进 /dep/packages
4. 如果你使用的版本与现有版本不同,还需要更新引用。方法见"更新 Nuget 包引用"一节。


## 从命令行构建 Terminal 包

Terminal 被打包为 `.msix`,由 `CascadiaPackage.wapproj` 项目生成。要从命令行构建该项目,可以运行以下命令(在已经运行过 `tools\razzle.cmd` 的窗口中):

```cmd
"%msbuild%" "%OPENCON%\OpenConsole.slnx" /p:Configuration=%_LAST_BUILD_CONF% /p:Platform=%ARCH% /p:AppxSymbolPackageEnabled=false /t:Terminal\CascadiaPackage /m
```

这一步耗时较长,而且只生成 `msix`,不会安装。要部署该包:

```powershell
# 如果还没有:
Import-Module .\tools\OpenConsole.psm1;
Set-MsBuildDevEnvironment;

# 调用 Set-MsBuildDevEnvironment 是为了找到 makeappx 的路径。
# 它运行起来也稍慢。如果你打算一直待在 PowerShell 里,最好先执行它。

Set-Location -Path src\cascadia\CascadiaPackage\AppPackages\CascadiaPackage_0.0.1.0_x64_Debug_Test;
if ((Get-AppxPackage -Name 'WindowsTerminalDev*') -ne $null) {
Remove-AppxPackage 'WindowsTerminalDev_0.0.1.0_x64__8wekyb3d8bbwe'
};
New-Item ..\loose -Type Directory -Force;
makeappx unpack /v /o /p .\CascadiaPackage_0.0.1.0_x64_Debug.msix /d ..\loose\;
Add-AppxPackage -Path ..\loose\AppxManifest.xml -Register -ForceUpdateFromAnyVersion -ForceApplicationShutdown
```

或者 cmd.exe 版本:
```cmd
@rem razzle.cmd 不会设置:
@rem set WindowsSdkDir=C:\Program Files (x86)\Windows Kits\10\
@rem vsdevcmd.bat 有大量逻辑来找这个路径。
@rem
@rem 下面我直接硬编码:

powershell -Command Set-Location -Path %OPENCON%\src\cascadia\CascadiaPackage\AppPackages\CascadiaPackage_0.0.1.0_x64_Debug_Test;if ((Get-AppxPackage -Name 'WindowsTerminalDev*') -ne $null) { Remove-AppxPackage 'WindowsTerminalDev_0.0.1.0_x64__8wekyb3d8bbwe'};New-Item ..\loose -Type Directory -Force;C:\'Program Files (x86)'\'Windows Kits'\10\bin\10.0.19041.0\x64\makeappx unpack /v /o /p .\CascadiaPackage_0.0.1.0_x64_Debug.msix /d ..\Loose\;Add-AppxPackage -Path ..\loose\AppxManifest.xml -Register -ForceUpdateFromAnyVersion -ForceApplicationShutdown
```

(是的,cmd 版本就是调用 PowerShell 去执行 PowerShell 版本。懒得手动转换剩下的部分,反正我都是从 `.vscode\tasks.json` 复制的)

在 VS 中构建包一开始就会生成松散布局(loose layout),然后注册松散清单,跳过 msix 这一步。很遗憾,它比这里的命令行内循环快得多。

### 2022 更新

以下命令可用于构建 Terminal 包然后部署。

```cmd
pushd %OPENCON%\src\cascadia\CascadiaPackage
bx
"C:\Program Files\Microsoft Visual Studio\2022\Preview\Common7\IDE\DeployAppRecipe.exe" bin\%ARCH%\%_LAST_BUILD_CONF%\CascadiaPackage.build.appxrecipe
popd
```

`bx` 只构建 Terminal 包,关键是它会生成 `CascadiaPackage.build.appxrecipe` 文件。构建完成后,就可以用 `DeployAppRecipe.exe` 以与 Visual Studio 相同的方式部署松散布局。

值得注意的是,这种构建 Terminal 包的方式无法利用 Visual Studio 的 FastUpToDate 检查,所以整个包的构建会明显变慢,因为 cppwinrt 在确认"已是最新、啥也不用干"之前要做大量工作。


### 看到 `DEP0700: Registration of the app failed` 了吗?

偶尔,我在 VS 中部署时会遇到 `DEP0700: Registration of the app failed.
[0x80073CF6] error 0x80070020: Windows cannot register the package because of an
internal error or low memory.`。对我们来说,这可能是因为 `OpenConsoleProxy.dll`
被锁住,正被某个其他终端包使用。

在 PowerShell 中执行等效命令可以获得更多信息:

```pwsh
Add-AppxPackage -register "Z:\dev\public\OpenConsole\src\cascadia\CascadiaPackage\bin\x64\Debug\AppX\AppxManifest.xml"
```

它会提示 `NOTE: For additional information, look for [ActivityId]
dbf551f1-83d0-0007-43e7-9cded083da01 in the Event Log or use the command line
Get-AppPackageLog -ActivityID dbf551f1-83d0-0007-43e7-9cded083da01`。那就照做:

```pwsh
Get-AppPackageLog -ActivityID dbf551f1-83d0-0007-43e7-9cded083da01
```

这会给你一大堆信息。在我的例子里,它揭示平台无法删除打包的 COM 注册项。关键行是:`AppX Deployment
operation failed with error 0x0 from API Logging data because access was denied
for file:
C:\ProgramData\Microsoft\Windows\AppRepository\Packages\WindowsTerminalDev_0.0.1.0_x64__8wekyb3d8bbwe,
user SID: S-1-5-18`

拿着这个路径执行:
```pwsh
sudo start C:\ProgramData\Microsoft\Windows\AppRepository\Packages\WindowsTerminalDev_0.0.1.0_x64__8wekyb3d8bbwe
```

(要用 `sudo`,否则路径被锁死)。进入 `PackagedCom` 文件夹,对
`OpenConsoleProxy.dll` 打开[File
Locksmith](https://learn.microsoft.com/en-us/windows/powertoys/file-locksmith)
(如果你更熟悉 Process Explorer 也行)。直接立刻以管理员身份重新启动它。
它会列出几个一直挂着的终端进程,把他们都结束掉。之后你应该就能正常部署了。
