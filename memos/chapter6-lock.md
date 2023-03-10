# 第六章 锁
## 6.1 竞态条件
* 当内存被并发的读取与写入时产生的状态。通常表现为更新丢失。
* 避免竞争的方法为使用锁，锁具有相互拍他性，在同一时间内通一个CPU中只有一个进程能获取锁，其他进程等待锁的获取。
* 锁保护数据是指通过保护不变量来实现。
* 可以将锁理解为序列化关键部分，即一次只能运行一个的方式保护不变量
* 锁能够保证正确性，但是限制性能。
* 锁的位置必须正确。

## 6.2 代码：Locks
* 自旋锁与睡眠锁 
    * 自旋锁的实现方式为无限循环访问锁的结构体，如果结构体中的锁字段为0，则表明可以获取锁。将锁字段的值设为1，并退出循环。如果锁字段不为1，则不断循环直到获得锁为止。
    * 锁的获取处理需要以原子的方式执行，amoswap指令能够以原子操作的方式将两个寄存器中的值交换。
    * C 库中的__sync_lock_test_and_set 函数在内部的实现使用的是amoswap指令
    * 锁的释放也需要原子的操作 使用amoswap指令呼唤两个值

## 6.3 代码： 使用锁
* 使用锁的几个原则
    * 在使用共享变量时使用锁。
    * 在需要保护不变量时使用。
* 何时不使用锁
    * 因为锁会降低并发性能，不需要锁可以解决时。

## 6.4 死锁与锁的排序
* 内核代码路径上需要同时持有多个锁时，要求所有的代码使用这些锁的顺序必须时相同的，否则会产生死锁。
* 锁的顺序与模块化
    * 锁的顺序使用会影响到模块化，因为需要知道函数中锁的使用细节。

## 6.5 可重入锁
* 如果一个进程尝试再一次获取同一个锁，则内核允许该进程获得。
* 可以一定程度上防止死锁的出现。
* 但是可重入锁破坏了关键区域需要执行原子化操作的原则。
* 只要锁的使用规则确定，可重入与锁的排序都可以工作。

## 6.6 锁与中断处理程序
* 中断处理程序与线程中会使用同一个锁来保护数据，中断处理程序执行的机制与线程之间执行的时机的原因会产生死锁。
    * clockintr 中会对ticks做加1处理时用tickslock上锁。
    * sys_sleep中会使用tickslock来上锁。但是sleep后的进程持有锁的前提下进入到睡眠状态，并等待clockintr来唤醒该进程。
    * 当clockintr执行到上锁处理时发现锁被占用并一直等待到锁的释放。锁的释放只有睡眠的进程唤醒后才能够执行。因此在会产生死锁。
* 解决方法：如果中断处理程序使用旋转锁，则CPU绝不能在启用中断的情况下保持该锁。
    * 在XV6中更加保守，上锁时会暂时停止产生中断。
    * 解锁时使中断能够再次产生。
    * 当如果存在多个已被上了锁的进程时，需要记录该嵌套的锁，只有在最后一个锁被释放后，有效化中断。

## 6.7 指令与内存访问顺序
* 在实际的CPU执行中指令的执行顺序会被打乱，即当两个指令之间无因果关系时CPU可能会先执行后一个指令。
* 在多核中因为并发的原因，有可能会产生与预期不一致的结果
* 内存模型：CPU的排序规则
* 内存栅栏：提示编译器与CPU不要在栅栏内的指令进行重新排序

## 6.8 睡眠锁
* 在等待时让出CPU，可以使其他线程执行。
* locked属性被自旋锁保护。