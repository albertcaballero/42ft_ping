#include "../ft_ping.h"

int main(int argc, char** argv){
    int flags = 0;
    char *ip = NULL;
    t_ping ping;

    for (int i = 0; i < argc; ++i){
		set_flags(argv[i], &flags);
	}
    ip = clean_args(argv, argc);
    ft_printf("Ping successful to %s\n", ip);

    ping.ip = ip;
}
