> 🌐 本文档由 [microsoft/terminal](https://github.com/microsoft/terminal) 翻译,英文原版见原项目。

# 为第三方工具添加配置文件

本文档旨在提供一份实用指南,帮助你在
[settings.json](https://docs.microsoft.com/en-us/windows/terminal/customize-settings/profile-settings) 文件中为常见第三方工具添加配置文件(profile)。

## Anaconda

假设你把 Anaconda 安装到了 `%USERPROFILE%\Anaconda3`:

```json
{
    "commandline": "cmd.exe /k \"%USERPROFILE%\\Anaconda3\\Scripts\\activate.bat %USERPROFILE%\\Anaconda3\"",
    "icon": "%USERPROFILE%\\Anaconda3\\Menu\\anaconda-navigator.ico",
    "name": "Anaconda3",
    "startingDirectory": "%USERPROFILE%"
}
```

## cmder

假设你把 cmder 安装到了 `%CMDER_ROOT%`:

```json
{
    "commandline": "cmd.exe /k \"%CMDER_ROOT%\\vendor\\init.bat\"",
    "name": "cmder",
    "icon": "%CMDER_ROOT%\\icons\\cmder.ico",
    "startingDirectory": "%USERPROFILE%"
}
```

## Cygwin

假设你把 Cygwin 安装到了 `C:\Cygwin`:

```json
{
    "name": "Cygwin",
    "commandline": "C:\\Cygwin\\bin\\bash --login -i",
    "icon": "C:\\Cygwin\\Cygwin.ico",
    "startingDirectory": "C:\\Cygwin\\bin"
}
```

注意,Cygwin 的起始目录之所以这样设置是为了让路径生效。由于 `--login` 标志,Cygwin 启动时默认打开的目录将是 `$HOME`。

## Far Manager

假设你把 Far 安装到了 `c:\Program Files\Far Manager`:

```json
{
    "name": "Far",
    "commandline": "\"c:\\program files\\far manager\\far.exe\"",
    "startingDirectory": "%USERPROFILE%",
    "useAcrylic": false
},
```

## Git Bash

假设你把 Git Bash 安装到了 `C:\\Program Files\\Git`:

```json
{
    "name": "Git Bash",
    "commandline": "C:\\Program Files\\Git\\bin\\bash.exe -li",
    "icon": "C:\\Program Files\\Git\\mingw64\\share\\git\\git-for-windows.ico",
    "startingDirectory": "%USERPROFILE%"
}
````

## Git Bash (WOW64)

假设你把 Git Bash 安装到了 `C:\\Program Files (x86)\\Git`:

```json
{
    "name": "Git Bash",
    "commandline": "%ProgramFiles(x86)%\\Git\\bin\\bash.exe -li",
    "icon": "%ProgramFiles(x86)%\\Git\\mingw32\\share\\git\\git-for-windows.ico",
    "startingDirectory": "%USERPROFILE%"
}
```

## MSYS2

假设你把 MSYS2 安装到了 `C:\\msys64`:

```json
{
    "name": "MSYS2",
    "commandline": "C:\\msys64\\msys2_shell.cmd -defterm -no-start -mingw64",
    "icon": "C:\\msys64\\msys2.ico",
    "startingDirectory": "C:\\msys64\\home\\user"
}
```

更多细节请参阅 MSYS2 文档中的[这一页](https://www.msys2.org/docs/terminals/#windows-terminal)。

## Visual Studio 开发人员命令提示

假设你安装了 VS 2019 Professional:

```json
{
    "name": "Developer Command Prompt for VS 2019",
    "commandline": "cmd.exe /k \"C:/Program Files (x86)/Microsoft Visual Studio/2019/Professional/Common7/Tools/VsDevCmd.bat\"",
    "startingDirectory": "%USERPROFILE%"
}
```

<!-- Adding a tool here? Make sure to add it in alphabetical order! -->
