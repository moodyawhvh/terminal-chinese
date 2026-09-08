> 🌐 本文档由 [microsoft/terminal](https://github.com/microsoft/terminal) 翻译,英文原版见原项目。

# 代码风格

## 理念
1. 如果是往现有类/函数里插入内容,尽量贴近现有风格。
1. 如果是全新代码或重构整个类/整个区域,请尽量采用 Modern C++ 风格,并尽可能参考 [C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines)。
1. 使用任何 Win32 或 NT API 时,请尽量使用 [Windows Implementation Library](./WIL.md) 的智能指针和结果处理器。
1. 不建议使用 NTSTATUS 作为返回码,优先使用 HRESULT 或异常。如果函数总是返回成功状态码,就不应返回状态码。任何返回状态码的函数都应标记 `noexcept` 并带有 `nodiscard` 属性。
1. 在 `TerminalApp` 中贡献代码时,注意正确使用 C++/WinRT 的[强引用与弱引用](https://docs.microsoft.com/en-us/windows/uwp/cpp-and-winrt-apis/weak-references),并充分理解 C++/WinRT 的[并发模型](https://docs.microsoft.com/en-us/windows/uwp/cpp-and-winrt-apis/concurrency)。
