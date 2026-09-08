> 🌐 本文档由 [microsoft/terminal](https://github.com/microsoft/terminal) 翻译,英文原版见原项目。

# 添加一个设置属性

1. 添加到 wincon.w
    * 这不在 OPENCONSOLE 里。提交 PR 时,务必同步更新 OS 仓库中的
      `.../console/published/wincon.w`。
      否则该分支无法构建。
    * 目前,你可以先把可用的修改更新到 winconp.h。
    * 定义注册表名(如 `CONSOLE_REGISTRY_CURSORCOLOR`)
    * 把该设置加入 `CONSOLE_STATE_INFO`。
    * 定义属性键 ID 和属性键本身。
        - 是的,绝大多数 `DEFINE_PROPERTYKEY` 定义都是一样的,只有 guid 的最后一个字节不同。

2. 在 Settings.hpp 中添加对应字段
    - 添加 getter、setter,全套流程。

3. 添加到属性表(propsheet)
    - 我们需要在属性表中*读取和写入*注册表,并*读取*快捷方式。是的,这很怪:属性表聪明到会复用 ShortcutSerialization::s_SetLinkValues,却不够聪明对 RegistrySerialization 做同样的事。
    - `src/propsheet/registry.cpp`
        -  `propsheet/registry.cpp@InitRegistryValues` 应初始化该属性的默认值。
        -  `propsheet/registry.cpp@GetRegistryValues` 应确保从注册表读取该属性。

4. 把该字段加入 propslib 的注册表映射。

5. 把该值加入 `ShortcutSerialization.cpp`
    - 在 `ShortcutSerialization::s_PopulateV2Properties` 中读取该值
    - 在 `ShortcutSerialization::s_SetLinkValues` 中写入该值

6. 把该设置加入 `Menu::s_GetConsoleState` 和 `Menu::s_PropertiesUpdate`
现在,你的新设置应当与其他所有属性一样被存储了。

7. 更新功能测试的属性,把该设置也加进去
    - `ft_uia/Common/NativeMethods.cs@WinConP`:
        - `Wtypes.PROPERTYKEY PKEY_Console_`。
        - `NT_CONSOLE_PROPS`。

8. 把该设置的默认值加入 `win32k-settings.man`
    - 如果该设置不应默认为 0 或 `nullptr`,你需要在 `win32k-settings.man` 中设置它的默认值。

9. 更新 `Settings::InitFromStateInfo` 和 `Settings::CreateConsoleStateInfo`,在 CONSOLE_STATE_INFO 中正确读取/写入该值。
