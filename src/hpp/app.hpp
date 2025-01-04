#ifdef __APP__

#include <stdio.h>

namespace App 
{

    void on_size_changed(void *arg);
    
    void loop(void *arg);

    int init_gl(void *arg);

    int init_modules(void *arg);

    int init(void *arg);

    void quit();

    void run(int argc, char **argv);

}

#endif
