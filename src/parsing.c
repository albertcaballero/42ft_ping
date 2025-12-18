
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

int parse_ipv4(char *ip){
    char **spl = ft_split(ip, '.');
    int i = 0;
    int count_p =0;
    for (int j = 0; ip[j]; ++j){
        if (ip[j] == '.')
            count_p++;
    }
    if (count_p != 3)
        return 0;

    for (i = 0; spl[i]; ++i){
        if (i > 4)
            return 0;
        int a = ft_atoi(spl[i]);
        if (a < 0 || a > 255)
            return 0;
    }
    if (i != 4)
        return 0;
    return 1;
}

char *clean_args(char **argv, int argc){
    int i = 1;
    for (i = 1; i < argc; ++i){
       if (argv[i][0] != '-')
           break;
    }
    if (!argv[i]){
        ft_dprintf(2, "ft_ping: usage error: Destination address required\n");
    	exit(2);
    }
    if (!parse_ipv4(argv[i])){
        ft_dprintf(2, "ft_ping: %s: Name or service not known\n", argv[i]);
    	exit(2);
    }
    return ft_strdup(argv[i]);
}
