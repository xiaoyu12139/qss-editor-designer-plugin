Qt designer编辑qss实时渲染加载

使用：

```
cmake --build build_msvc2019_64 --config Release
```

将build_msvc2019_64\Release\LiveQssDesignerPlugin.dll粘贴到C:\Qt\5.15.2\msvc2019_64\plugins\designer，然后启动

C:\Qt\5.15.2\msvc2019_64\bin\designer.exe，在from窗口标题栏上右键操作

![image-20251113165513493](https://github.com/xiaoyu12139/qss-editor-designer-plugin/blob/master/img/image-20251113165513493.png)

先绑定qss文件，然后开启热加载。修改qss文件后保存文件（ctrl+s）即可即时刷新
