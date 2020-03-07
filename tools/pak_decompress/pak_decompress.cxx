#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <getopt.h>
#include <zlib.h>

static const int32_t CHUNK = 65536;

static const struct option arg_table[] = {
    { "input", required_argument, nullptr, 'i' },
    { "output", required_argument, nullptr, 'o' },
    { "help", no_argument, nullptr, 'h' },
    { "verbose", no_argument, nullptr, 'v' },
};

struct Config {
    std::string input;
    std::string output;
    bool help;
    bool verbose;
};

int32_t inf(std::istream &is, std::ostream &os, const bool verbose)
{
    z_stream strm;
    int32_t ret = 0;
    uint32_t have = 0;
    Bytef in[CHUNK];
    Bytef out[CHUNK];
    char check;

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit(&strm);

    if (ret != Z_OK)
        return ret;

    is.get(check);
    if (check == 'x')
    {
        if (verbose)
            std::cout << "found zlib file" << std::endl;
        is.seekg(0, std::ios::beg);
    }
    else
    {
        is.seekg(16, std::ios::beg);
        is.get(check);
        if (check == 'x')
        {
            if (verbose)
                std::cout << "found Neocron 1 zlib compressed file" << std::endl;
            is.seekg(16, std::ios::beg);
        }
        else
        {
            if (verbose)
                std::cerr << "error: not a compatible zlib compressed file" << std::endl;
            return -1;
        }
    }

    do
    {
        strm.avail_in = is.readsome(reinterpret_cast<char *>(in), CHUNK);
        if (!is.good())
        {
            inflateEnd(&strm);
            return Z_ERRNO;
        }
        if (strm.avail_in == 0)
            break;
        strm.next_in = in;

        do
        {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);
            assert(ret != Z_STREAM_ERROR);

            switch (ret)
            {
                case Z_NEED_DICT:
                    ret = Z_DATA_ERROR;
                    [[fallthrough]];
                case Z_DATA_ERROR:
                    [[fallthrough]];
                case Z_MEM_ERROR:
                    inflateEnd(&strm);
                    return ret;
            }

            have = CHUNK - strm.avail_out;
            if (!os.write(reinterpret_cast<char *>(out), have).good())
            {
                inflateEnd(&strm);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);
        assert(strm.avail_in == 0);
    } while (ret != Z_STREAM_END);

    return ret;
}

void zerr(const int32_t ret)
{
    std::cerr << "zpipe: ";
    switch (ret)
    {
        case Z_ERRNO:
            if (!std::cin.good())
                std::cerr << "error reading stdin" << std::endl;
            if (!std::cout.good())
                std::cerr << "error writing stdout" << std::endl;
            break;
        case Z_STREAM_ERROR:
            std::cerr << "invalid compression level" << std::endl;
            break;
        case Z_DATA_ERROR:
            std::cerr << "invalid or incomplete deflate data" << std::endl;
            break;
        case Z_MEM_ERROR:
            std::cerr << "out of memory" << std::endl;
            break;
        case Z_VERSION_ERROR:
            std::cerr << "zlib version mismatch" << std::endl;
    }
}

void help(const char *name)
{
    std::cout << "usage: " << name << " <options>\n"
              << "Decompresses a single Neocron 1 pak file.\n"
              << "\n"
              << "options:\n"
              << "  -h, --help           show this help screen\n"
              << "  -v, --verbose        show more details\n"
              << "  -i, --input <file>   pak file to decompress\n"
              << "  -o, --output <file>  decompressed file\n"
              << std::endl;
}

void parseCommandLine(const int32_t argc, char **argv, Config &config)
{
    bool process_args = true;

    while (process_args)
    {
        int32_t index = 0;
        int32_t argument = 0;

        argument = getopt_long(argc, argv, "hvi:o:", arg_table, &index);
        switch (argument)
        {
            case 'h':
                config.help = true;
                break;

            case 'v':
                config.verbose = true;
                break;

            case 'i':
                config.input = optarg;
                break;

            case 'o':
                config.output = optarg;
                break;

            case -1:
            default:
                process_args = false;
        }
    }

    if (config.output.empty())
        config.output = config.input + ".decompressed";
}

int32_t main(int32_t argc, char **argv)
{
    Config config = { "", "", false, false };
    std::ifstream ifile;
    std::ofstream ofile;
    int32_t ret = 0;

    parseCommandLine(argc, argv, config);
    ifile.open(config.input, std::ios::binary);
    ofile.open(config.output, std::ios::binary);

    if (ifile.is_open() && ifile.good() && ofile.is_open() && ofile.good())
    {
        const size_t in_size = ifile.seekg(0, std::ios::end).tellg();
        size_t out_size = 0;

        ifile.seekg(0, std::ios::beg);
        ret = inf(ifile, ofile, config.verbose);
        out_size = ofile.tellp();

        ifile.close();
        ofile.close();

        if ((ret == Z_OK) && (config.verbose == true))
        {
            std::cout << "bytes read:        " << in_size << std::endl;
            std::cout << "bytes written:     " << out_size << std::endl;
            std::cout << "compression ratio: " << (100 - ((in_size * 100) / out_size)) << "%"
                      << std::endl;
        }
        else if ((ret == Z_OK) && (config.verbose == false))
            std::cout << "[OK]    " << config.input << std::endl;
        else if ((ret != Z_OK) && (config.verbose == false))
            std::cout << "[ERROR] " << config.output << std::endl;
    }
    else
        std::cout << "[ERROR] " << "unable to open files" << std::endl;

    return ret;
}
