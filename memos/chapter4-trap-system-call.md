# 第四章 陷阱与系统调用
* 4.1 RISC-V 陷阱
    * 以下的控制寄存器用于内核通知CPU如何处理陷阱时使用。
        * **stvec** 存储陷阱处理函数的地址，当发生需要处理陷阱时从stvec中读取。
        * **sepc** 当陷阱发生时，RISC-V保存调入陷阱之前的程序计数器的寄存器。当sret（从陷阱处理函数返回到用户程序时）将sepc的写入到程序计数器PC中。
        * **scause** RISC-V 引起掉入陷阱的原因的数字保存在该寄存器中
        * **sstatus** 在sstatus中的SIE比特位控制devide intterups（设备中断）是否有效。如果内核晴空SIE，则RISC-V将推迟设备中断，直到内核设置SIE，SPP比特位表名陷阱时来自用户模式还是监管者模式，sret指令用该寄存器来返回相应的模式。

    *  当发生陷阱时（除了计时器中断）RISC-V执行以下操作
        * 如果是设备中断，切sstatus的SIE比特位是清空的，则不做任何处理
        * 通过清空SIE比特位，无效设备中断。
        * 复制pc的值到sepc
        * 保存当前的模式到sstatus的SPP位中
        * 设定scause
        * 转换到监管者模式
        * 复制stvec到pc中
        * 执行新的PC
    
    * 在调入陷阱时CPU不会切换到内核的页表。理由是实现安全方面的隔离

* 4.2来自用户空间的陷阱
    * RISC-V中硬件不会切换页表，因此在用户空间的页表中存在stvec（陷阱处理函数的程序计数器）的值。
    * 回想用户空间内存的空间布局，在用户空间内存的最高的位附近（TRAMPOLINE）保存了陷阱处理函数。
        * 在XV6中stvec寄存器指向该区域。
        * 内核内存页表与用户内存页表都有TRAMPOLIENE的映射，且虚拟内存的值相同。
        * 在uservec指向的用户陷阱处理函数中保存了32个通用集村的值到陷阱栈桢中。
            * 陷阱栈桢在用户空间内存的TRAMPOLINE的低一个页的位置中。（TRAPFRAME）
        * trapframe（进程的属性）保存着当前进程的内核栈的空间地址，需要将sp（栈顶地址寄存器）舍只为kernel-stack的值
    * 在用户陷阱处理函数中需要处理三种情况
        * 系统调用，
        * 设备中断
        * 异常
        
    * 详细细节参考4.2节
        * [4.2节细节](http://xv6.dgs.zone/tranlate_books/book-riscv-rev1/c4/s2.html)

* 4.3 调用system call 代码
    * 系统调用会讲调用系统函数的番号写入到a7寄存器中，并调用ecall指令。
    * ecall指令会将模式调换到监视者模式，并且调用stvec中指向的陷阱处理函数

* 4.4 系统调用参数：代码
    * 内核函数argint argaddr argfd 通过指定参数的位置与指针，获取用户调用时传递的参数
    * fetchstr函数可以获取用户空间传递的字符传
    * 内核对物理内存时直接映射的，因此可以通过walk寻找到用户空间传递的虚拟内存的物理地址，并可以对其直接修改。

* 4.5 来自内核的陷阱
    * 当运行在内核时stvec指向的是kernelvec
    * kernelvec保存32个通用寄存器的值到内核栈中。并跳转到kerneltrap中。
    * 内核陷阱处理函数中处理两种情况
        * 设备中断
        * 异常。

* 4.6 缺页异常

    * 缺页异常
        * 缺页异常发生在页表条目的有效位（PTE_V）为空或读取位（PTE_R），写入位（PTE_W），执行位（PTE_X），模式位（PTE_U）未被设定却想要执行时。
        * 有三种缺页异常
            * 加载页异常：当加载指令无法翻译其虚拟地址时
            * 存储页异常：当存储指令无法翻译其虚拟地址时
            * 指令页异常：当程序计数器的地址无法被翻译时
        * **scause**寄存器指示页缺失的类型
        * **stval**寄存器包含无法被翻译的地址

    * 写时复制 （ copy on write COW fork）
        * 当fork时，子进程会完全复制父进程的内存，但是如果完全复制一份内存是一种浪费，因为子进程并不会完全使用复制过来的内存。
        * 一种有效的方式是仅复制父进程的页表，物理内存先不复制。并且对父进程的页表和子进程的页表设定为只读（PTE_W清空）。此时父进程与子进程共享同一个物理内存区域
        * 当其中某一个进程发生写入指令时，会发生写入页异常。内核将修改发生缺页进程的页表，将页表指向新的物理地址并从父进程复制后修改PTE_W为可写入，并重新执行指令。
        * 为了能够正确的清理物理内存，需要对引用共享物理内存的虚拟内存进行维护。使用页表条目中RSW位来维护。

    * 延迟分配 （lazy allocation）
        * 第一： 当应用程序请求堆区的内存时调用sbrk，通知内核增大堆区大小，但是并不分配实际的物理内存，且不创建页表条目。
        * 第二：当新地址发生缺页时，内核分配物理内存并且与新地址进行映射。
        
    * demmand paging

        * 思路
            * 将一小部分用户页存储在内存中，剩余的部分存储在磁盘中（页区域）。
            * 内核将存储在磁盘中的页表条目设定为无效（PTE_V无效）
            * 当需要使用无效位的的数据时，引发缺页异常，此时内核将磁盘上的内容写入到内存中，并且将页表条目指定到内存区域中。
        * 当发生页需要被映射到内存中，却没有多余的内存可以分配时需要将原来的内存进行驱逐。
            * 驱逐原则
                * 驱逐在一段时间内未被修改的页（PTE_A位记录），因为页一旦被访问则PTE_A位永远保持有效，因此需要定期的清理，该清理的方法为**时钟算法**
                * 时钟算法
                    * 定期清理PTE_A位，为LRU做内存驱逐的基础

        * 
    