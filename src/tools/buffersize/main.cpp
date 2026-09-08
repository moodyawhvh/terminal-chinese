// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
//
// 【中文注释】buffersize 工具说明:
// 查询当前控制台屏幕缓冲区/视口(viewport)大小,然后用 VT 转义序列在
// 缓冲区四周画一圈彩色边框(顶部 '-'、底部 '='、两侧 'L'/'R',
// 每个格子颜色递进),用来直观验证缓冲区尺寸与重排行为。

#include <windows.h>
#include <wil/Common.h>
#include <wil/result.h>
#include <wil/resource.h>
#include <wil/wistd_functional.h>
#include <wil/wistd_memory.h>
#include <cstdlib> /* srand, rand */
#include <ctime> /* time */

#include <deque>
#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include <cassert>

using namespace std;
////////////////////////////////////////////////////////////////////////////////
// 【中文注释】全局状态:标准输入/输出句柄。
HANDLE hOut;
HANDLE hIn;

// 【中文注释】拼接 CSI 转义序列:"ESC [" + seq。
std::string csi(std::string seq)
{
    std::string fullSeq = "\x1b[";
    fullSeq += seq;
    return fullSeq;
}

// 【中文注释】直接打印一个 CSI 序列。
void printCSI(std::string seq)
{
    printf("%s", csi(seq).c_str()); // save cursor
}

// 【中文注释】打印 CUP(光标定位)序列,把光标移到逻辑坐标 (x, y)
// (VT 坐标从 1 开始,故各 +1)。
void printCUP(int x, int y)
{
    printf("\x1b[%d;%dH", y + 1, x + 1); // save cursor
}

// 【中文注释】设置 256 色背景色(bg 为色号)。
void print256color(int bg)
{
    printf("\x1b[48;5;%dm", bg); // save cursor
}

// bin\x64\Debug\buffersize.exe
// 【中文注释】主入口:
// 1. 开启 VT 处理(ENABLE_VIRTUAL_TERMINAL_PROCESSING)并关闭自动回车;
// 2. 用 GetConsoleScreenBufferInfoEx 拿到视口宽高(注意:resize 事件本身
//    并不携带我们想要的完整信息,所以这里主动查询);
// 3. 保存光标位置,画顶部/底部边框,再逐行画左右边框,颜色逐格递进;
// 4. 最后恢复光标位置与颜色属性。
int __cdecl wmain(int /*argc*/, WCHAR* /*argv[]*/)
{
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    hIn = GetStdHandle(STD_INPUT_HANDLE);

    DWORD dwMode = 0;
    THROW_LAST_ERROR_IF(!GetConsoleMode(hOut, &dwMode));
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    dwMode |= DISABLE_NEWLINE_AUTO_RETURN;
    // THROW_LAST_ERROR_IF_FALSE(SetConsoleMode(hOut, dwMode));
    SetConsoleMode(hOut, dwMode);
    // the resize event doesn't actually have the info we want.
    CONSOLE_SCREEN_BUFFER_INFOEX csbiex = { 0 };
    csbiex.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
    bool fSuccess = !!GetConsoleScreenBufferInfoEx(hOut, &csbiex);
    if (fSuccess)
    {
        SMALL_RECT srViewport = csbiex.srWindow;
        short width = srViewport.Right - srViewport.Left + 1;
        short height = srViewport.Bottom - srViewport.Top + 1;

        std::string topBorder = std::string(width, '-');
        std::string bottomBorder = std::string(width, '=');

        int color = 17;
        const int colorStep = 1;
        printf("Buffer size is wxh=%dx%d\n", width, height);
        printCSI("s"); // save cursor
        printCSI("H"); // Go Home
        print256color(color);
        color += colorStep;
        printf("%s", topBorder.c_str());
        printCUP(0, height - 1);
        print256color(color);
        color += colorStep;
        printf("%s", bottomBorder.c_str());

        for (int y = 1; y < height - 1; y++)
        {
            printCUP(0, y);
            print256color(color);
            color += colorStep;
            printf("L");

            printCUP(width - 1, y);
            print256color(color);
            color += colorStep;
            printf("R\n");
        }

        printCSI("u"); // restore cursor
        printCSI("m"); // restore color
    }

    return 0;
}
