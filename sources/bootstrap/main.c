
int main(int argc, char **argv);
void __entry_point()
{
    main(0, (char **)0);
}
int main(int argc, char **argv)
{
    (void)(argc);
    (void)(argv);

    const char *v = "hello world\n";
    asm volatile(
        "mov $0, %%rax \n"
        "syscall \n" ::"b"(v)
        : "memory");
    while (1)
    {
    }

    return 0;
}
