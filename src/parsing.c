
#include "../ft_ping.h"


// -l, long format F_LONG
// -R, --recursive, recursive  F_RECURS
// -a, --all, show files starting with . (hidden) F_ALL
// -r, --reverse, reverse order F_REVERSE
// -t, sort by time, newest first F_TIME


void set_flags(char* arg, int *flags){
	if (!arg || ft_strlen(arg) == 0){
		return;
	}
	if (arg[0] != '-'){
		return;
	}
	for (int i = 1; arg[i] != 0; ++i){
		switch (arg[i]){
			case 'v':
				*flags |= F_VERBOSE;
				break;
			case 'h':
			case '?':
				*flags |= F_HELP;
				break;
			default:
				ft_dprintf(2, "ft_ping: invalid option -- '%c'\n", arg[i]);
				exit(2);
				break;
		}
	}
}

char *clean_args(char **argv, int argc){
    int count = 0;
    int i = 0;
    for (i = 0; i < argc; ++i){
       if (argv[i][0] != '-')
           break;
    }

    ft_dprintf(2, "ft_ping: %s: Name or service not known\n", argv[i]);
	exit(2);
    return ft_strdup(argv[i]);
}

int parse_ipv4(char *ip){
    char **spl = ft_split(ip, '.');
    if ()
}
