#include <iostream>
#include <string>     // std::string, std::stoi

// /home/aljosa/image%d.png
int main(int argc, char *argv[])
{
//    // Three params: path, startFrame, endFrame

    if (argc<4) {
        std::cout << "Error!" << std::endl;
    }

    int startFrame = std::stoi(std::string(argv[2]));
    int endFrame = std::stoi(std::string(argv[3]));

    for (int i=startFrame; i<endFrame; i++) {
        char buff[500];

        sprintf(buff, argv[1], i);

        std::cout << buff << std::endl;

    }
    return 0;
}
