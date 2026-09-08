> 🌐 本文档由 [microsoft/terminal](https://github.com/microsoft/terminal) 翻译,英文原版见原项目。

# OpenConsole 的分支

在 OpenConsole 中,`dev/main` 是仓库的主分支。

任何以 `dev/` 开头的分支都会被我们的 CI 系统识别,并自动运行 x86 和 amd64 构建,以及单元测试和功能测试。功能分支的命名模式是 `dev/<别名>/<随便你写什么>`,例如 `dev/austdi/SomeCoolUnicodeFeature`。关键是 dev 前缀和你的别名。

`inbox` 是一个特殊分支,用于把 OpenConsole 代码协调进主 OS 仓库。

代码会被检入 OS 仓库的 `/onecore/windows/core/console/open`。谨慎起见,请确保你提交的修改能在 razzle 中构建该目录。

# 代码提交流程

因为我们在 OS 仓库之外构建,所以需要一种方式把合并进 `dev/main` 的代码送回 OS 仓库。做法是:PR 合并(最好已 squash)进 `dev/main` 后,将其 cherry-pick 到 `inbox` 分支。我们有一个叫 Git2Git 的工具,它监听 `inbox` 的新合并,并把提交复制到 OS 仓库。你可以自己批准并完成 `inbox` 的 PR。`inbox` PR 提交大约一分钟后,Git2Git 会在 OS 仓库以别名 `miniksa` 创建一个 PR。它会自动指向我们当时使用的 OS 分支,只需要你去批准并完成它。该合并完成后,最好用新代码构建一次 OS 分支,确保这个 PR 不会成为当晚构建失败的元凶。

## cherry-pick 到 inbox 失败时怎么办

有时 VSTS 不允许 cherry-pick 到 inbox 分支。它可能有正当理由,也可能只是闹脾气。你需要在本地机器上手动完成合并。步骤如下:

1. 确保你已拉取 `dev/main` 和 `inbox` 分支的最新提交
2. 从 inbox 新建一个分支
3. 把 PR 中的提交 cherry-pick 到新分支(如果你合并进 `dev/main` 时做了 squash,这一步会容易得多)
4. 解决所有合并冲突并提交
5. 把新分支推送到远端
6. 用该分支在 `inbox` 新建一个 PR
7. 完成 PR,然后继续完成 OS 仓库中自动创建的 PR
