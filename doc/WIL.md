> 🌐 本文档由 [microsoft/terminal](https://github.com/microsoft/terminal) 翻译,英文原版见原项目。

# Windows Implementation Library

## 概述
[Windows Implementation Library](https://github.com/Microsoft/wil),简称 WIL,是一个纯头文件库,旨在让 Windows API 的使用更可预测、(希望)更少 Bug。

大多数函数位于 `wil::` 或 `wistd::` 命名空间。`wistd::` 用于那些在 STL 的 `std::` 命名空间中有对应物、但带有特殊功能(如无异常)的东西。其余都在 `wil::` 命名空间。

目前为止,我们在代码中对 WIL 的主要用法有……

### 智能指针 ###

在 [wil/resource.h](https://github.com/microsoft/wil/blob/master/include/wil/resource.h) 中,为许多 Windows OS 资源提供了智能指针式的类,如文件句柄、套接字句柄、进程句柄等。它们形如 `wil::unique_handle`,在超出作用域时会调用相应/匹配的 OS 函数(这里是 `CloseHandle()`)。

另一个实用的东西是 `wil::make_unique_nothrow()`,它类似于 `std::make_unique`(但没有异常,这有助于你与控制台中现存的免异常代码整合)。它返回 `wistd::unique_ptr`(而非 `std::unique_ptr`),用法类似。

### 结果处理 ###

为了管理从 Windows API 返回的各类结果码,[wil/result.h](https://github.com/microsoft/wil/blob/master/include/wil/result.h) 提供了大量宏。

举例来说,`DuplicateHandle()` 返回一个 `BOOL` 值,失败时为 `FALSE`,并要求你从操作系统 `GetLastError()` 查询真实结果码。这种情况下,你可以用宏 `RETURN_IF_WIN32_BOOL_FALSE` 包装对 `DuplicateHandle()` 的调用,它会自动处理这个模式,并在失败时返回等价的 `HRESULT`。

由此形成了漂亮的模式:你可以把函数中的所有资源都交给 `std::unique_ptr` 或各种 `wil::` 智能指针/智能句柄保护,然后在每次调用 Windows API 时使用 `RETURN_IF_*`,即可保证任何失败情形下资源都会被正确清理。注意,这通常要求你以 `HRESULT` 作为返回码,并用出参指针返回数据。当然也有例外……详情请阅读头文件。

使用这种模式的另一个好处是:任何时点的失败都会记录到我们的全局跟踪/调试通道,可在调试器输出中查看,并附带错误发生的确切行号和函数信息。

此外,如果你只是想确保失败情形被记录下来用于调试,所有这些宏都有对应的 `LOG_IF_*` 版本,只记录失败然后继续执行。
