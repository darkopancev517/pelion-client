#include <stdint.h>

extern "C" int wisun_main_system_init(void);

int main(void)
{
    wisun_main_system_init();

    while (1);

    return 0;
}
