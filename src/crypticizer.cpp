#include <cstdio>
#include <iostream>

int main(int argc, char** argv)
{
    if (argc > 2)
    {
        std::cout << "Usage: " << argv[0] << " [session name]" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
