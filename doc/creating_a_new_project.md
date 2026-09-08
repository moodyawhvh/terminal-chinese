> 🌐 本文档由 [microsoft/terminal](https://github.com/microsoft/terminal) 翻译,英文原版见原项目。

# 创建新项目

## 创建新的 WinRT 组件 DLL 并在其他项目中引用它

创建新 DLL 时,参考现有 DLL 的 `.vcxproj`(如 `TerminalControl.vcxproj`)非常有帮助。虽然你大体上应该照抄现有 `.vcxproj` 的内容,但过程中有几件事需要反复确认。

- [ ] 确保在 vcxproj 的_顶部_ `<Import>` 我们的 pre props,在_底部_引入 post props。
```
<!-- pre props -->
<Import Project="..\..\..\common.openconsole.props" Condition="'$(OpenConsoleDir)'==''" />
<Import Project="$(OpenConsoleDir)src\cppwinrt.build.pre.props" />

<!-- everything else -->

<!-- post props -->
<Import Project="$(OpenConsoleDir)src\cppwinrt.build.post.props" />
```
- [ ] 在 `WindowsTerminal.vcxproj` 和 `TerminalApp.vcxproj` 中都为你的新 `.vcxproj` 添加 `<ProjectReference>`
- [ ] 在 `TerminalAppLib.vcxproj` 中添加类似这样的 `<Reference>`:
```
    <Reference Include="Microsoft.Terminal.NewDLL">
      <HintPath>$(OpenConsoleCommonOutDir)\TerminalNewDLL\Microsoft.Terminal.NewDLL.winmd</HintPath>
      <IsWinMDFile>true</IsWinMDFile>
      <Private>false</Private>
      <CopyLocalSatelliteAssemblies>false</CopyLocalSatelliteAssemblies>
    </Reference>
```
- [ ] 确保项目带有包含以下内容的 `.def` 文件。`WINRT_GetActivationFactory` 这一段很重要,它对外暴露新 DLL 的激活工厂,让其他项目能成功调用该 DLL 的 `GetActivationFactory` 来获取 DLL 中的类。
```
EXPORTS
DllCanUnloadNow = WINRT_CanUnloadNow                    PRIVATE
DllGetActivationFactory = WINRT_GetActivationFactory    PRIVATE
```
- 关于整个流程再多说一点背景:`AppXManifest.xml` 文件定义了哪些类属于哪些 DLL。如果你的项目想要类 `X.Y.Z`,它可以在清单定义中查到该类来自 `X.Y.dll`,然后加载该 DLL,并调用名为 `GetActivationFactory(L"X.Y.Z")` 的特定函数来拿到想要的类。因此,`AppXManifest` 中的定义是激活正常工作的_必要条件_,我经常需要反复检查该文件,确认预期的定义都在。
- _注_:如果你的新库最终作为对我们 Centennial 打包项目 `CascadiaPackage` 的引用汇总进去,你不必手动把定义加进 `AppXManifest.xml`,因为 Centennial 打包项目会自动枚举 WinMD 引用树并把信息拼进 `AppXManifest.xml`。但如果你的新项目_不会_最终汇总到自动把引用写入 `AppXManifest` 的打包项目,你就得手动添加。

### 故障排查
- 如果你遇到这样的错误:
    ```
    X found processing metadata file ..\blah1\Microsoft.UI.Xaml.winmd, type already exists in file ..\blah\NewDLLProject\Microsoft.UI.Xaml.winmd.
    ```
    说明 `Microsoft.UI.Xaml.winmd` 不该出现在输出文件夹里却出现了。试着在 `.vcxproj` 顶部加上这个块:
    ```
    <ItemDefinitionGroup>
    <Reference>
        <Private>false</Private>
    </Reference>
    </ItemDefinitionGroup>
    ```
    这会让所有引用默认变为非私有,意思是"别拷贝到我的文件夹里"。

- 如果你遇到 `Class not Registered` 错误,可能是某个类没有在应用清单中注册。可以去检查 `src/cascadia/CascadiaPackage/bin/x64/Debug/AppX/AppXManifest.xml`,看是否存在指向你新建 DLL 各类的条目。如果引用不在,请再次确认你已向 `WindowsTerminal.vcxproj` 和 `TerminalApp.vcxproj` 都添加了 `<ProjectReference>` 块。

- 如果你遇到类似 `Error in the DLL` 这种极其含糊的错误,并且在该行之前注意到新 DLL 刚被加载就立刻被卸载,请再次确认新 DLL 的定义出现在 `AppXManifest.xml` 中。如果你的新 DLL 是作为汇总到 `CascadiaPackage` 的项目的引用被包含进来,请再次确认你为该项目创建了 `.def` 文件。否则,如果你的新项目_不会_汇总到会为你填充 `AppXManifest` 引用的包,你就得自己添加这些引用。
