# shell web server
    用于在web浏览器中，远程执行单条shell指令（或许可以执行shell脚本？）
## 用法
    目前只适用于android源码编译，可以放到external中，然后在源码根目录mmm external/adb_shell_web_server
    编译可能会报错，因为mk文件中prebuilts/ndk/current/sources/cxx-stl/stlport/stlport/和prebuilts/ndk/current/sources/cxx-stl/stlport/libs/armeabi/*.a这两个路径可能有偏差
    编译完成后，直接push到system/bin下通过adb执行/system/bin/wisp，或者放到/tmp目录下
    通过浏览器输入以下内容就可以执行shell了：
    http://X.X.X.X:8888/?shell=env

## 已知bug
    目前只是个简单版本，很多地方未完善。已知bug：
     1.shell指令如果不存在或其他报错，会返回shell dont return any result,maybe return errors!