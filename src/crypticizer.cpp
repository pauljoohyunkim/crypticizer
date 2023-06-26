#include <cstdio>
#include <iostream>

int main(int argc, char** argv)
{
    /*
     * Usage: crypticizer [session path]
     * where session path is the directory of the project.
     * */
    if (argc > 2)
    {
        std::cout << "Usage: " << argv[0] << " [session path]" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
