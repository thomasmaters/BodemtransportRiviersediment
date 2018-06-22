#include <math.h>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <thread>

#include <sys/stat.h>

#define BYTESCHUNCKTOREAD 20

std::size_t getFileSize(std::string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        std::cerr << "Not enough arguments given, expected 4 got " << (argc - 1) << "." << std::endl;
        std::cerr << "Usages: [input file][output file][rate in mb/s]" << std::endl;
        return EXIT_FAILURE;
    }

    // Open inputstream and read everything as binary.
    std::ifstream testDataInputStream(argv[1], std::ios::in | std::ios::binary);
    // Open outputstream which discards old information and outputs as binary.
    std::ofstream testDataOutputStream(argv[2], std::ios::out | std::ios::trunc | std::ios::binary);
    double throughput        = std::atof(argv[3]);
    std::size_t bytePerWrite = throughput * 1024 * 1024;

    // Can we open our input and output file.
    if (!testDataInputStream)
    {
        std::cerr << "Failed to open input file '" << argv[1] << "'" << std::endl;
        return EXIT_FAILURE;
    }
    if (!testDataOutputStream)
    {
        std::cerr << "Failed to open output stream" << std::endl;
        return EXIT_FAILURE;
    }

    std::size_t fileSize = getFileSize(argv[1]);

    char *buffer = new char[fileSize];
    if (testDataInputStream.read(buffer, fileSize))
    {
        std::cout << "Loaded inputfile into buffer of size: " << fileSize << std::endl;
    }
    else
    {
        return EXIT_FAILURE;
    }

    std::cout << "system_clock" << std::endl;
    std::cout << std::chrono::system_clock::period::num << std::endl;
    std::cout << std::chrono::system_clock::period::den << std::endl;
    std::cout << "steady = " << std::boolalpha << std::chrono::system_clock::is_steady << std::endl << std::endl;

    std::cout << "high_resolution_clock" << std::endl;
    std::cout << std::chrono::high_resolution_clock::period::num << std::endl;
    std::cout << std::chrono::high_resolution_clock::period::den << std::endl;
    std::cout << "steady = " << std::boolalpha << std::chrono::high_resolution_clock::is_steady << std::endl
              << std::endl;

    std::cout << "steady_clock" << std::endl;
    std::cout << std::chrono::steady_clock::period::num << std::endl;
    std::cout << std::chrono::steady_clock::period::den << std::endl;
    std::cout << "steady = " << std::boolalpha << std::chrono::steady_clock::is_steady << std::endl << std::endl;

    std::size_t frameCounter = 0;
    std::chrono::time_point<std::chrono::steady_clock> subStart;
    std::chrono::time_point<std::chrono::steady_clock> subEnd;
    std::chrono::duration<int64_t, std::chrono::_V2::steady_clock::period>::rep subDuration;
    auto start = std::chrono::steady_clock::now();

    while (true)
    {
        if ((frameCounter + 1) * bytePerWrite > fileSize)
        {
            // Performance numbers.
            auto end            = std::chrono::steady_clock::now();
            auto duration       = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            double inSeconds    = duration / 1000.0;
            double bytesWritten = std::floor(fileSize / bytePerWrite) * bytePerWrite;
            std::cout << std::endl;
            std::cout << "Written " << bytesWritten << " bytes" << std::endl;
            std::cout << "in " << inSeconds << " seconds" << std::endl;
            std::cout << "Deviation: " << (throughput - bytesWritten / inSeconds / 1024 / 1024) / throughput * 100
                      << std::endl;
            std::cout << "Written output file with: " << 1.0 / inSeconds * bytesWritten / 1024 / 1024 << " mB/s"
                      << std::endl;
            std::cout << "DONE" << std::endl;
            break;
        }
        subStart = std::chrono::steady_clock::now();

        // Actual writing to file.
        if (!testDataOutputStream.write((char *)&buffer[frameCounter * bytePerWrite], bytePerWrite))
        {
            break;
        }
        // Does this line make writing slower?
        testDataOutputStream << std::flush;
        frameCounter++;

        subEnd      = std::chrono::steady_clock::now();
        subDuration = std::chrono::duration_cast<std::chrono::milliseconds>(subEnd - subStart).count();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 - subDuration));
    }

    delete buffer;

    return EXIT_SUCCESS;
}
