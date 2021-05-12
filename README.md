# TLPI_learn_note
《LINUX/LINUX系统编程手册》学习记录和练习习题解答。

## Index:
+ [环境搭建](#环境搭建)
+ [CH3 系统编程概念](#ch3-系统编程概念)
+ [CH4 文件I/O：通用的I/O模型](#ch4-文件io通用的io模型)
+ [CH5 深入探究文件I/O](#ch5-深入探究文件io)
+ [CH6 进程](#ch6-进程)
+ [CH7 内存分配](#CH7-内存分配)
+ [CH8 用户和组](#ch8-用户和组)
+ [CH9 进程凭证](#ch9-进程凭证)
+ [CH10 时间](#CH10-时间)
+ [CH11 系统限制和选项](#CH11-系统限制和选项)
+ [CH12 系统和进程信息](#CH12-系统和进程信息)
+ [CH29 线程：介绍](#CH29-线程介绍)


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
<br>
<br>

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
<br>
<br>

## CH4 文件I/O：通用的I/O模型
### [4.1代码](./Exercise/4_1.c)
没什么难度，就是getopt函数学了半天，恼了

### [4.2代码](./Exercise/4_2.c)
有一个问题，就是无法真正的区分源文件中的连续0或是真实的连续空洞。以前的系统中的cp命令类似这样实现的，并没有真正的做到复制空洞。但是现在的(我用的Ubuntu 20.04)已经能处理这种情况，完成正常复制空洞文件了，但是作为练习这种程度已经差不多了，我现在也看不懂源码。。。

---
<br>
<br>

## CH5 深入探究文件I/O
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

### [5.7](./Exercise/5_7.c)
1. open文件O_CREAT要用八进制或是直接用宏，我一直用十进制。。。服了我自己了。。。🤮
2. 别再误删了。。。不要直接用vscode在ssh下删除，因为真的找不回，最好alias rm变成移动到某个文件夹，因为误删真的太恐怖了，多commit，不用push
3. 实现倒是很简单，没啥要说的,另外用不用tlpi_hdr.h根本无所谓，我单纯是为了少些几行代码
---
<br>
<br>

## CH6 进程
### 6.1
bss区只记录需要多大的空间，但是并不实际分配空间给它，我ll和du看了，确实占用的空间不关bss的事。<br>
拓展:而数据段是要实际分配空间的，我给全局变量```int a[100000000] = {1}```，虽然只有第一个元素是1，但这个数组也算是初始化了(不过={0}仍然算在bss区，不过这个无所谓啦)，然后ll和du了一些可执行文件，确实变的很大。

### 6.2
```
#include <stdio.h>
#include <setjmp.h>

jmp_buf env;

void func()
{
    longjmp(env, 1);
}

int main()
{
    if (setjmp(env) == 0)
    {
        func();
    }

    return 0;
}
```

### 6.3
[我的简陋实现](./Exercise/6_3.c)
---
<br>
<br>

## CH7 内存分配
### 7.1
可以看见增长幅度挺大的，多次malloc只需执行一次sbrk系统调用，来提高效率，这种思路非常非常常见。(比如vector等等)
![IMG](./IMG/1.png)
![IMG](./IMG/2.png)

### 7.2
这个完全就是CMU15-213的malloc lab，在那个lab中要求你编写一个动态内存申请器（malloc，free，realloc），[我的实现代码](https://github.com/sunhuiquan/csapp_lab/blob/main/malloclab-handout/mm.c)。
我的思路是隐式分离空闲链表，所谓隐式的意思是使用大小来定位前后的块(指针形式叫显式)，分离的意思是有多个有着不同大小范围的链表，每个链表维持着一组在一定范围大小的(2^n-1~2^n)的块，使用2的幂，<<1来快速找到合适的。
合并的思路是看空闲标志位，free的时候也许前后都是空闲，来合并，根据合并后的大小插入合适的链表中。

---
<br>
<br>

## CH8 用户和组
### 8.1
显然这个问题本身错了，作者也在勘误中修改了，因为这里的(long)getpwnam("name")->pw_uid是传递值直接给%ld了，后面的又不会改变这个值。
```
拓展一下：
    当一个函数带有多个参数时，C/C++语言没有规定在函数调用时实参的求值顺序。而编译器根据对代码进行优化的需要自行规定对实参的求值顺序。
    有的编译器规定自左至右，有的编译器规定自右至左，这种对求值顺序的不同规定，对一般参数来讲没有影响。但是，如果实参表达式中带有副作
    用的运算符时，就有可能产生由于求值顺序不同而造成了二义性。
    同样的对于参数有多个函数调用的情况，函数调用的顺序也未定义。
```

我改写一下代码来体先不可重入性。
![IMG](./IMG/3.png)

### 8.2
非常简单，[代码](./Exercise/8_2.c)。

---
<br>
<br>

## CH9 进程凭证
### 9.1
```
   real  effective  saved  fs
a. 1000  0          0      0
b. 1000  2000       2000   2000
c. 1000  2000       0      2000
d. 1000  0          0      2000
e. 1000  2000       3000   2000
```

### 9.2
不具有，因为特权只看有效用户ID(不过因为real是0很容易回到特权模式)

### 9.3
[my_initgroups.c](./Exercise/9_3.c)
```
使用set-root-user-ID，来让程序有权限读取密码文件
gcc 9_3.c -Wall
sudo chown root:water a.out
sudo chmod u+s a.out
```

这个很有意思，因为get和setgroups()函数针对的是调用者ID(即属主即实际的ID)，然后initgroups还会指定用户名，所以说进程的切换也是挺有意思的，不过C风格字符串的操作真是让人头大。

### 9.4
```
int old_eff = geteuid();
（1）
seteuid(getuid()); // 暂时有效切换到实际
seteuid(old_eff);  // 恢复，因为保存UID的值允许
// 注意多种实现完全都可以，另外setuid和getuid完全作用不同

（2）
setreuid(getuid(),getuid())     // 永久放弃，r不是-1(即使值没变)
// 也会修改保存UID变成有效UID(当然是设置完后的新有效UID)
```

### 9.5
```
int old_eff = geteuid();
（1）
seteuid(getuid()); // 暂时有效切换到实际
seteuid(old_eff);  // 恢复，因为保存UID的值允许
//此时setuid不能了，因为是特权用户，会把保存UID也改了，所以最好一直用seteuid好用

（2）
setreuid(getuid(),getuid())     // 永久放弃，r不是-1(即使值没变)
// 也会修改保存UID变成有效UID(当然是设置完后的新有效UID)
```

---
<br>
<br>

## CH10 时间
### 10.1
times的单位是sysconf(_SC_CLK_TCK), (2^32 - x) / sysconf(_SC_CLK_TCK)<br>
clock的单位是CLOCKS_PER_SEC, (2^32 - x) / CLOCKS_PER_SEC

---
<br>
<br>

## CH11 系统限制和选项
### 11.1
孩子只有linux系统(ㄒoㄒ)

### 11.2
ext4 file system:
```
_PC_NAME_MAX, 255
_PC_PATH_MAX, 4096
_PC_PIPE_BUF, 4096
```

---
<br>
<br>

## CH12 系统和进程信息
其实如果不是读取/proc下的文件的话，sysinfo/procfs_pidmax.c是有bug的，因为write的字节数如果少于之前的，那么只是单纯覆盖一部分，而剩下的仍然是原来的值，比如99999写123，变成12399；当然这里并不是bug，因为读取的/proc其实是个伪文件系统，并不是普通的文件。不过这写法真的是让我困扰了好一阵。。。

### 12.1
[代码](./Exercise/12_1.c)本身不难，但是C的处理字符串和繁杂程度倒是有点恶心，另外量有点多。。。

### 12.2
[代码](./Exercise/12_2.c)在上一问的基础上改了改完成，另外想要做的好看太难了。

### 12.3
[代码](./Exercise/12_3.c)又是在上一问的基础上修修改改，读写目录，读写链接技能get🤮。

---
<br>
<br>

## CH13 文件I/O缓冲

---
<br>
<br>

## CH14 文件系统

---
<br>
<br>

## CH15 文件属性

---
<br>
<br>

## CH16 扩展属性

---
<br>
<br>

## CH17 访问控制列表

---
<br>
<br>

## CH18 目录与链接

---
<br>
<br>

## CH19 监控文件事件

---
<br>
<br>


## CH29 线程：介绍

### 29.1
ERROR [EDEADLK/EDEADLOCK Resource deadlock avoided]这是在linux下发生的；当然其他UNIX系统可能会发生死锁（因为pthread_join(pthread_self(),NULL)会阻塞直到等待的线程退出，而这个等待的线程正是它本身。它本身需要执行完pthread_join才可以退出，而pthread_join解除阻塞又需要退出，显然矛盾，发生死锁。）
```
if(!pthread_equal(tid, pthread_self())) // 确保tid不是该线程自己的POSIX thread id
    pthread_join(tid, NULL); // 另外一定是要用pthread_equal来确保可移植性
```

### 29.2
主线程退出后主线程栈的内存之后会被重用，而创建的线程使用的指针指向已经被重用的原主线程栈的内存空间，这种行为未定义，可能造成严重后果

---
<br>
<br>

## CH30 线程：线程同步

### 30.1
[代码](./Exercise/30_1.c)

### 30.2

---
<br>
<br>

## CH31 线程：线程安全和每线程存储

### 31.1

---
<br>
<br>
