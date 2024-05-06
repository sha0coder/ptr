#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>


struct module *find_module_by_name(const char *name, struct list_head *modules_head) {
    struct module *mod;

    list_for_each_entry(mod, modules_head, list) {
        if (strcmp(mod->name, name) == 0) {
            return mod;
        }
    }

    return NULL;
}

