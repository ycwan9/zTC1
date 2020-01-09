# 使用JTAG看串口日志

新建  C:\Users\zogodo\.gdbinit 文件, 写入一下内容:

```shell
add-auto-load-safe-path D:\code\C\mico\zTC1\.gdbinit
set auto-load safe-path /
```

执行以下命令:

```shell
mico make TC1@MK3031@moc debug
(gdb) b SetLogRecord
(gdb) commands 1
> silent
> p log
> p \n
> c
> end
(gdb) set height 0
(gdb) c

```

参考: <https://www.tablix.org/~avian/blog/archives/2012/08/monitoring_serial_console_through_jtag/>