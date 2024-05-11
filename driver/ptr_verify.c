#include <linux/sched.h>
#include <linux/mm.h>


char verify_pointer(void *ptr) {
    struct task_struct *task = current;
    struct mm_struct *mm = task->mm;
    struct vm_area_struct *vma;

    if (!mm) {
        printk(KERN_INFO "there is no task->mm");
        return 0;
    }

    //down_read(&vma_lock);

    vma = mm->mmap;
    while (vma != NULL) {
        if ((unsigned long)ptr >= vma->vm_start && (unsigned long)ptr < vma->vm_end-8) {
            //up_read(&vma_lock);
            return 1;
        }
        vma = vma->vm_next;
    }

    //up_read(&vma_lock);
    return 0;
}
