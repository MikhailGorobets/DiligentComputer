<a name="cloning"></a>
# Cloning the Repository

This is the master repository that contains four [submodules](https://git-scm.com/book/en/v2/Git-Tools-Submodules). To get the repository and all submodules, use the following command:

```
git clone --recursive https://github.com/DiligentGraphics/DiligentEngine.git
```

When updating existing repository, don't forget to update all submodules:

```
git pull
git submodule update --recursive
```
