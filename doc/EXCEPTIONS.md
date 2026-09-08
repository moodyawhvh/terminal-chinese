> 🌐 本文档由 [microsoft/terminal](https://github.com/microsoft/terminal) 翻译,英文原版见原项目。

# 使用异常

## 理念
把异常引入一个现有的非异常代码库可能相当危险。控制台最初用 C 编写,那时 C++ 在 Windows 操作系统中还很少使用。作为 Windows 控制台现代化项目的一部分,我们转换到了 C++,但仍然排斥在代码中使用基于异常的错误处理,担心它会引入意想不到的故障。然而,STL 和类似的库实在好用,有时使用它们会明显更简单。有鉴于此,我们在考虑使用异常时遵循一套规则。

## 规则
1. **不要**让异常从新代码泄漏进旧代码
1. **要**视情况使用 `NTSTATUS` 或 `HRESULT` 作为返回值(优先 `HRESULT`)
1. **要**把所有异常行为封装在实现类内部
1. **不要**向旧代码引入现代的抛异常代码。而是按需重构以实现封装,或使用非异常代码
1. **要**使用 WIL 作为非抛出式现代设施的替代(如 `wil::unique_ptr<>`)

## 示例

### 把异常行为封装在类中

    class ExceptionsDoNotLeak
    {
        public:
        HRESULT SomePublicFunction();
        int iPublic;

        private:
        void _SomePrivateFunction();
        int _iPrivate;
    };

### 使用 WIL 提供非抛出式现代设施

TODO
