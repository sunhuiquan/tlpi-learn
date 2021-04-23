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

### [5.7](./Exercise/5_7.c)
1. open文件O_CREAT要用八进制或是直接用宏，我一直用十进制。。。服了我自己了。。。🤮
2. 别再误删了。。。不要直接用vscode在ssh下删除，因为真的找不回，最好alias rm变成移动到某个文件夹，因为误删真的太恐怖了，多commit，不用push
3. 实现倒是很简单，没啥要说的,另外用不用tlpi_hdr.h根本无所谓，我单纯是为了少些几行代码
---


## CH6 进程
### 6.1
---


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

## CH8 用户和组
### 8.1
显然这个问题本身错了，作者也在勘误中修改了，因为这里的(long)getpwnam("name")->pw_uid是传递值直接给%l了，后面的又不会改变这个值。
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

## CH10 时间
### 10.1
times的单位是sysconf(_SC_CLK_TCK), (2^32 - x) / sysconf(_SC_CLK_TCK)
clock的单位是CLOCKS_PER_SEC, (2^32 - x) / CLOCKS_PER_SEC
---

## CH11 系统限制和选项
