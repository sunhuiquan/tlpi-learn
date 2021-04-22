# TLPI_learn_note
《LINUX/LINUX系统编程手册》学习记录和练习习题解答。

## 环境搭建
```
    wget https://man7.org/tlpi/code/download/tlpi-201025-dist.tar.gz
    tar -zxvf tlpi-201025-dist.tar.gz
    cd tlpi-dist/
    make
```

```
    cd lib/
    sudo cp tlpi_hdr.h /usr/local/include/
    sudo cp get_num.h /usr/local/include/
    sudo cp error_functions.h /usr/local/include/
    sudo cp ename.c.inc /usr/local/include/
```

如果没有静态库的话需要手动创建,现在的新版本并不需要这一步，所以这一步不做
```
    g++ -c get_num.c error_functions.c
    ar -crv libtlpi.a get_num.o error_functions.o
    sudo cp libtlpi.a /usr/local/lib
```

运行需要链接libtlpi.a静态库
```
    g++ main.cpp -o main -ltlpi
```
---

## CH3 系统编程概念
### [3.1代码](./Exercise/3_1.c)
```
    #include <unistd.h>
    #include <sys/reboot.h>

    int main(){
        reboot(RB_AUTOBOOT);
        return 0;
    }
```
注意使用root权限。RB_AUTOBOOT 是定义在reboot.h头文件中的。如果我们换成RB_POWER_OFF 则系统直接关机。

在其他的Linux中，我们也可以使用如下的程序进行重启。(然而我的Ubuntu 20.04发行版运行不了，只可以上面方式运行，不过这是发行版和具体版本的问题，毕竟书都过了六七年了。)

```
    #include <unistd.h>
    #include <linux/reboot.h>

    int main(){
        reboot(LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2A, LINUX_REBOOT_CMD_RESTART, 0);
        return 0;
    }
```
这里的LINUX_REBOOT_MAGIC1、LINUX_REBOOT_MAGIC2A参数在linux/reboot.h中被定义,这些参数是Linux的作者Linus Torvalds自己和他三个女儿的生日。

我又稍微研究了一下，发现warning: implicit declaration of function，linux/reboot.h这个头文件没有声明reboot函数，出于别的考虑吧，再说应该用glibc的reboot比较好吧。写下声明，这样main函数内的调用就能找到reboot函数了。
```
    #include <unistd.h>
    #include <linux/reboot.h>

    int reboot(int);
    int main()
    {
        reboot(LINUX_REBOOT_CMD_RESTART);
        return 0;
    }
```
---

## CH4 文件I/O：通用的I/O模型
### [4.1代码](./Exercise/4_1.c)
没什么难度，就是getopt函数学了半天，恼了

### [4.2代码](./Exercise/4_2.c)
有一个问题，就是无法真正的区分源文件中的连续0或是真实的连续空洞。以前的系统中的cp命令类似这样实现的，并没有真正的做到复制空洞。但是现在的(我用的Ubuntu 20.04)已经能处理这种情况，完成正常复制空洞文件了，但是作为练习这种程度已经差不多了，我现在也看不懂源码。。。
---

## CH5
### 勘误
首先说下第五章开头那句所有系统调用都以原子方式执行这句话是是错的，只是大部分完全符合原子性，另外一些需要别的措施来保证同步，还有一些系统调用会被信号中断，还有部分中断的系统调用恢复的情况，不能一概而论。而且原子性需要看语义上下文，可以看这个链接有帮助。
https://www.zhihu.com/question/46552411/answer/130482168

### 5.1
要么在所有头文件前(头文件里可能对这个宏有需要)#define _FILE_OFFSET_BITS 64，要么编译的时候-D_FILE_OFFSET_BITS 64一样的效果，由于电脑本身就是64位，所以看不出效果，如果是32位的系统那么就可以突破文件大小2GB的限制了。

### 5.2
当然会在最后写入咯，因为APPEND之后write当然是先改变偏移量到最后，再写入，当然这是一组原子性的动作。(当然read不受影响)

### 5.3
多个进程同时写同一个文件的情况，如果没有APPEND，那么一个进程刚刚write后但在更新偏移量前，另一个进程可能在当前偏移量写，从而覆盖，造成文件变小。(代码太简单就没写)

### [5.4](./Exercise/5_4.c)
话说APUE也有这道题欸，能用fcntl果然舒服多了，虽然那个是直接用dup貌似更简单。。。

### 5.5
当然共享，太简单不做了。

### 5.6
书上印刷错误，是" world",所以是Gidday world

### 5.7
1.open文件O_CREAT要用八进制或是直接用宏，我一直用十进制。。。服了我自己了。。。🤮
2.别再误删了。。。不要直接用vscode在ssh下删除，因为真的找不回，最好alias rm变成移动到某个文件夹，因为误删真的太恐怖了，多commit，不用push

