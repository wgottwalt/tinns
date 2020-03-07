#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

int32_t main(int32_t argc, char **argv)
{
    struct FileHeader {
        uint32_t id;
        uint32_t offset;
        uint32_t csize;
        uint32_t dsize;
        uint32_t name_len;
        std::string name;
    };

    if (argc == 2)
    {
        if (std::ifstream ifile(argv[1]); ifile.is_open() && ifile.good())
        {
            uint32_t fcount = 0;

            ifile.seekg(4, std::ios::beg);
            ifile.read(reinterpret_cast<char *>(&fcount), sizeof (fcount));

            if (fcount)
            {
                FileHeader files[fcount];

                std::cout << "-- id ------------------------------------------ name ---- csize "
                          << "----- size --" << std::endl;
                for (auto &file : files)
                {
                    ifile.read(reinterpret_cast<char *>(&file.id), sizeof (file.id));
                    ifile.read(reinterpret_cast<char *>(&file.offset), sizeof (file.offset));
                    ifile.read(reinterpret_cast<char *>(&file.csize), sizeof (file.csize));
                    ifile.read(reinterpret_cast<char *>(&file.dsize), sizeof (file.dsize));
                    ifile.read(reinterpret_cast<char *>(&file.name_len), sizeof (file.name_len));
                    file.name.resize(file.name_len, ' ');
                    ifile.read(file.name.data(), file.name_len);

                    std::cout << std::setw(5) << file.id << " "
                              << std::setw(48) << file.name << " "
                              << std::setw(10) << file.csize << " "
                              << std::setw(10) << file.dsize << std::endl;
                }
            }

            ifile.close();
        }
    }
    else
        std::cout << "usage: " << argv[0] << " <nc1 vfs file>" << std::endl;

    return 0;
}
