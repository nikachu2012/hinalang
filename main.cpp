#include <cstdio>
#include <fstream>

int main(void)
{
    std::ifstream f("sample.hina");
    if (!f)
    {
        printf("file open error\n");
        return 0;
    }

    f.close();
}
