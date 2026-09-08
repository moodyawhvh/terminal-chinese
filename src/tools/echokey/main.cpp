// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
//
// 【中文注释】echokey 工具说明:
// 该工具从控制台输入缓冲区读取按键事件(KEY_EVENT_RECORD / WINDOW_BUFFER_SIZE_RECORD),
// 把每个按键的详细字段(是否按下、重复次数、虚拟键码、扫描码、字符、控制键状态)
// 原样打印到屏幕,用于测试和验证终端的输入(尤其是 VT 输入模式)行为。
// 按 Ctrl+D 退出。
// 可用选项:-i 启用 VT 输入;-o 关闭 VT 输出;-w 读取窗口事件;
//           -a 使用 ReadConsoleInputA(ANSI);--alt 使用备用缓冲区。

#define DEFINE_CONSOLEV2_PROPERTIES

// System headers
#include <windows.h>

// Standard C++ library
#include <cwchar>
#include <cstdlib>
#include <cstdio>

#include <string>
using namespace std;

// WIL
#define WIL_SUPPORT_BITOPERATION_PASCAL_NAMES
#include <wil/Common.h>
#include <wil/Result.h>

// 【中文注释】全局开关:是否启用 VT 输入/输出、是否读取窗口事件、
// 是否使用备用缓冲区、是否使用 ANSI(A)版 API。
bool gVtInput = false;
bool gVtOutput = true;
bool gWindowInput = false;
bool gUseAltBuffer = false;
bool gUseAscii = false;

// 【中文注释】主循环退出标志:收到 Ctrl+D 时置位。
bool gExitRequested = false;

HANDLE g_hOut = INVALID_HANDLE_VALUE;
HANDLE g_hIn = INVALID_HANDLE_VALUE;

static const char CTRL_D = 0x4;

// 【中文注释】拼接并输出一个 CSI 序列(ESC '[' + seq)。
// 若 VT 输出被关闭(-o),则直接丢弃,不打印任何转义序列。
void csi(string seq)
{
    if (!gVtOutput)
    {
        return;
    }
    string fullSeq = "\x1b[";
    fullSeq += seq;
    printf(fullSeq.c_str());
}

void useAltBuffer()
{
    csi("?1049h");
}

void useMainBuffer()
{
    csi("?1049l");
}

// 【中文注释】把字符转成可打印形式(两个字符宽):
// ESC 打成 "^[",Ctrl+C 打成 "^C",\0 \r \n \t \b 打成 "\0" 等转义写法,
// 其余字符后补一个空格,保证屏幕上每个字符占两列,便于阅读。
void toPrintableBufferA(char c, char* printBuffer, int* printCch)
{
    if (c == '\x1b')
    {
        printBuffer[0] = '^';
        printBuffer[1] = '[';
        printBuffer[2] = '\0';
        *printCch = 2;
    }
    else if (c == '\x03')
    {
        printBuffer[0] = '^';
        printBuffer[1] = 'C';
        printBuffer[2] = '\0';
        *printCch = 2;
    }
    else if (c == '\x0')
    {
        printBuffer[0] = '\\';
        printBuffer[1] = '0';
        printBuffer[2] = '\0';
        *printCch = 2;
    }
    else if (c == '\r')
    {
        printBuffer[0] = '\\';
        printBuffer[1] = 'r';
        printBuffer[2] = '\0';
        *printCch = 2;
    }
    else if (c == '\n')
    {
        printBuffer[0] = '\\';
        printBuffer[1] = 'n';
        printBuffer[2] = '\0';
        *printCch = 2;
    }
    else if (c == '\t')
    {
        printBuffer[0] = '\\';
        printBuffer[1] = 't';
        printBuffer[2] = '\0';
        *printCch = 2;
    }
    else if (c == '\b')
    {
        printBuffer[0] = '\\';
        printBuffer[1] = 'b';
        printBuffer[2] = '\0';
        *printCch = 2;
    }
    else
    {
        printBuffer[0] = (char)c;
        printBuffer[1] = ' ';
        printBuffer[2] = '\0';
        *printCch = 2;
    }
}
// 【中文注释】toPrintableBufferA 的宽字符(Unicode)版本,逻辑完全一致。
void toPrintableBufferW(wchar_t c, wchar_t* printBuffer, int* printCch)
{
    if (c == L'\x1b')
    {
        printBuffer[0] = L'^';
        printBuffer[1] = L'[';
        printBuffer[2] = L'\0';
        *printCch = 2;
    }
    else if (c == '\x03')
    {
        printBuffer[0] = L'^';
        printBuffer[1] = L'C';
        printBuffer[2] = L'\0';
        *printCch = 2;
    }
    else if (c == '\x0')
    {
        printBuffer[0] = L'\\';
        printBuffer[1] = L'0';
        printBuffer[2] = L'\0';
        *printCch = 2;
    }
    else if (c == '\r')
    {
        printBuffer[0] = L'\\';
        printBuffer[1] = L'r';
        printBuffer[2] = L'\0';
        *printCch = 2;
    }
    else if (c == '\n')
    {
        printBuffer[0] = L'\\';
        printBuffer[1] = L'n';
        printBuffer[2] = L'\0';
        *printCch = 2;
    }
    else if (c == '\t')
    {
        printBuffer[0] = L'\\';
        printBuffer[1] = L't';
        printBuffer[2] = L'\0';
        *printCch = 2;
    }
    else if (c == '\b')
    {
        printBuffer[0] = L'\\';
        printBuffer[1] = L'b';
        printBuffer[2] = L'\0';
        *printCch = 2;
    }
    else
    {
        printBuffer[0] = (wchar_t)c;
        printBuffer[1] = L' ';
        printBuffer[2] = L'\0';
        *printCch = 2;
    }
}

// 【中文注释】ANSI 版按键事件处理:打印按键全部字段;
// 松开(bKeyDown=0)的按键用灰色打印以示区分;收到 Ctrl+D 时请求退出。
void handleKeyEventA(KEY_EVENT_RECORD keyEvent)
{
    char printBuffer[3];
    int printCch = 0;
    const char c = keyEvent.uChar.AsciiChar;
    toPrintableBufferA(c, printBuffer, &printCch);

    if (!keyEvent.bKeyDown)
    {
        // Print in grey
        csi("38;5;242m");
    }

    wprintf(L"Down: %d Repeat: %d KeyCode: 0x%x ScanCode: 0x%x Char: %hs (0x%x) KeyState: 0x%x\r\n",
            keyEvent.bKeyDown,
            keyEvent.wRepeatCount,
            keyEvent.wVirtualKeyCode,
            keyEvent.wVirtualScanCode,
            printBuffer,
            keyEvent.uChar.AsciiChar,
            keyEvent.dwControlKeyState);

    // restore colors
    csi("0m");

    // Die on Ctrl+D
    if (keyEvent.uChar.AsciiChar == CTRL_D)
    {
        gExitRequested = true;
    }
}

// 【中文注释】Unicode 版按键事件处理,逻辑与 ANSI 版一致,读取 uChar.UnicodeChar。
void handleKeyEventW(KEY_EVENT_RECORD keyEvent)
{
    wchar_t printBuffer[3];
    int printCch = 0;
    const wchar_t c = keyEvent.uChar.UnicodeChar;
    toPrintableBufferW(c, printBuffer, &printCch);

    if (!keyEvent.bKeyDown)
    {
        // Print in grey
        csi("38;5;242m");
    }

    wprintf(L"Down: %d Repeat: %d KeyCode: 0x%x ScanCode: 0x%x Char: %s (0x%x) KeyState: 0x%x\r\n",
            keyEvent.bKeyDown,
            keyEvent.wRepeatCount,
            keyEvent.wVirtualKeyCode,
            keyEvent.wVirtualScanCode,
            printBuffer,
            keyEvent.uChar.UnicodeChar,
            keyEvent.dwControlKeyState);

    // restore colors
    csi("0m");

    // Die on Ctrl+D
    if (c == CTRL_D)
    {
        gExitRequested = true;
    }
}

// 【中文注释】窗口缓冲区大小变化事件:打印新缓冲区尺寸,
// 并查询 CONSOLE_SCREEN_BUFFER_INFOEX 得到当前视口的位置与宽高。
void handleWindowEvent(WINDOW_BUFFER_SIZE_RECORD windowEvent)
{
    SHORT bufferWidth = windowEvent.dwSize.X;
    SHORT bufferHeight = windowEvent.dwSize.Y;

    CONSOLE_SCREEN_BUFFER_INFOEX csbiex = { 0 };
    csbiex.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
    bool fSuccess = !!GetConsoleScreenBufferInfoEx(g_hOut, &csbiex);
    if (fSuccess)
    {
        SMALL_RECT srViewport = csbiex.srWindow;

        unsigned short viewX = srViewport.Left;
        unsigned short viewY = srViewport.Top;
        unsigned short viewWidth = srViewport.Right - srViewport.Left + 1;
        unsigned short viewHeight = srViewport.Bottom - srViewport.Top + 1;
        wprintf(L"BufferSize: (%d,%d) Viewport:(x, y, w, h)=(%d,%d,%d,%d)\r\n",
                bufferWidth,
                bufferHeight,
                viewX,
                viewY,
                viewWidth,
                viewHeight);
    }
}

// 【中文注释】Ctrl 处理器:吞掉 Ctrl+C / Ctrl+Break,避免工具被直接中断,
// 让 Ctrl+D 成为唯一退出方式。
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
    switch (fdwCtrlType)
    {
    // Handle the CTRL-C signal.
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
        return true;
    }

    return false;
}

// 【中文注释】打印用法帮助。
void usage()
{
    wprintf(L"usage: echokey [options]\n");
    wprintf(L"options:\n");
    wprintf(L"\t-i: enable reading VT input mode.\n");
    wprintf(L"\t-o: disable VT output.\n");
    wprintf(L"\t-w: enable reading window events.\n");
    wprintf(L"\t-a: Use ReadConsoleInputA instead.\n");
    wprintf(L"\t--alt: run in the alt buffer. Cannot be combined with `-o`\n");
    wprintf(L"\t-?: print this help message\n");
}

// 【中文注释】主入口:解析命令行参数 → 保存原始控制台模式 →
// 按选项设置 VT 输入/输出、窗口输入等标志位 → 进入读取循环逐条处理
// INPUT_RECORD → 退出时恢复备用缓冲区与原始控制台模式。
int __cdecl wmain(int argc, wchar_t* argv[])
{
    gVtInput = false;
    gVtOutput = true;
    gWindowInput = false;
    gUseAltBuffer = false;
    gExitRequested = false;
    gUseAscii = false;

    for (int i = 1; i < argc; i++)
    {
        wstring arg = wstring(argv[i]);
        wprintf(L"arg=%s\n", arg.c_str());
        if (arg.compare(L"-i") == 0)
        {
            gVtInput = true;
            wprintf(L"Using VT Input\n");
        }
        else if (arg.compare(L"-w") == 0)
        {
            gVtInput = true;
            wprintf(L"Reading Window Input\n");
        }
        else if (arg.compare(L"--alt") == 0)
        {
            gUseAltBuffer = true;
            wprintf(L"Using Alt Buffer.\n");
        }
        else if (arg.compare(L"-o") == 0)
        {
            gVtOutput = false;
            wprintf(L"Disabling VT Output\n");
        }
        else if (arg.compare(L"-a") == 0)
        {
            gUseAscii = true;
            wprintf(L"Using ReadConsoleInputA\n");
        }
        else if (arg.compare(L"-?") == 0)
        {
            usage();
            exit(0);
        }
        else
        {
            wprintf(L"Didn't recognize arg `%s`\n", arg.c_str());
            usage();
            exit(0);
        }
    }

    g_hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    g_hIn = GetStdHandle(STD_INPUT_HANDLE);

    DWORD dwOutMode = 0;
    DWORD dwInMode = 0;
    GetConsoleMode(g_hOut, &dwOutMode);
    GetConsoleMode(g_hIn, &dwInMode);
    SetConsoleCtrlHandler(CtrlHandler, TRUE);
    const DWORD initialInMode = dwInMode;
    const DWORD initialOutMode = dwOutMode;

    if (gVtOutput)
    {
        dwOutMode = WI_SetAllFlags(dwOutMode, ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN);
    }

    if (gVtInput)
    {
        dwInMode = WI_SetFlag(dwInMode, ENABLE_VIRTUAL_TERMINAL_INPUT);
    }

    if (gWindowInput)
    {
        dwInMode = WI_SetFlag(dwInMode, ENABLE_WINDOW_INPUT);
    }

    if (gUseAltBuffer && !gVtOutput)
    {
        wprintf(L"Specified `--alt` to use the alternate buffer with `-o`, which disables VT.  --alt requires VT output to be enabled.\n");
        Sleep(2000);
        exit(EXIT_FAILURE);
    }

    SetConsoleMode(g_hOut, dwOutMode);
    SetConsoleMode(g_hIn, dwInMode);

    if (gUseAltBuffer)
    {
        useAltBuffer();
    }

    wprintf(L"Start Mode (i/o):(0x%4x, 0x%4x)\n", initialInMode, initialOutMode);
    wprintf(L"New Mode   (i/o):(0x%4x, 0x%4x)\n", dwInMode, dwOutMode);
    wprintf(L"Press ^D to exit\n");

    while (!gExitRequested)
    {
        INPUT_RECORD rc;
        DWORD dwRead = 0;

        if (gUseAscii)
        {
            ReadConsoleInputA(g_hIn, &rc, 1, &dwRead);
        }
        else
        {
            ReadConsoleInputW(g_hIn, &rc, 1, &dwRead);
        }

        switch (rc.EventType)
        {
        case KEY_EVENT:
        {
            if (gUseAscii)
            {
                handleKeyEventA(rc.Event.KeyEvent);
            }
            else
            {
                handleKeyEventW(rc.Event.KeyEvent);
            }
            break;
        }
        case WINDOW_BUFFER_SIZE_EVENT:
        {
            handleWindowEvent(rc.Event.WindowBufferSizeEvent);
            break;
        }
        }
    }

    if (gUseAltBuffer)
    {
        useMainBuffer();
    }
    SetConsoleMode(g_hOut, initialOutMode);
    SetConsoleMode(g_hIn, initialInMode);

    exit(EXIT_FAILURE);
}
