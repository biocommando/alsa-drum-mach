int get_int_arg(int argc, char **argv, const char *arg, int default_value)
{
    int i;
    for (i = 0; i < argc && strcmp(argv[i], arg); i++);
    if (i >= argc - 1)
    	return default_value;
    sscanf(argv[i + 1], "%d", &i);
    return i;
}
