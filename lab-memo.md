# syscall 实验笔记
## System call tracing
* 完成trace的功能，需要能够指定mask参数，指定要跟踪的指令
    * 该课程体系下在用户模式调用系统函数的过程如下
        * 在user.h  中追加trace函数的声明
        * 在usys.pl 中追加调用内核模式下执行的sys_trace的函数，在这个pl中生成生成调用sys_trace汇编代码，使得用户模式下调用的trace，会在该汇编中得到执行，并通过ecall 函数来调用内核函数
        * ecall指令会执行kerner的syscall函数，并通过process的结构体获得该进程下的信息（传递的参数等）
        * syscall 根据a7寄存器值（进程结构体中有保存） 来调用相应的内核函数。
        * 追加一个sys_trace的函数取得的mask后，写入到process结构体的变量的trace_mask（新追加）中使得能够在其他系统调用中也能够得到该值。
        * 在fork的实现的上下文拷贝的部分中，需要追加一个process结构题的trace_mask复制的处理
    
    * 要点
        * 系统通过进程，虚拟内存等实现，完成了进程与内存的隔离。
        * 在该题的实现中需要通过process来保持上下文。