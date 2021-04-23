#include <tlpi_hdr.h>
#include <pwd.h>

struct passwd *my_getpwnam(const char *name)
{
    struct passwd *pwd;
    while ((pwd = getpwent()) != NULL)
        if (strcmp(pwd->pw_name, name) == 0)
            break;
    endpwent();
    return pwd;
}

int main()
{
    printf("%ld\n", (long)my_getpwnam("water")->pw_uid);

    return 0;
}