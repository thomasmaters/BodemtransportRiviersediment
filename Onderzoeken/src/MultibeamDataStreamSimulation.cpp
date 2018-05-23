#include <iostream>
#include <iomanip>
#include <fstream>
#include <thread>
#include <chrono>
#include <ctime>

#include <sys/stat.h>

#define BYTESCHUNCKTOREAD 42

std::size_t getFileSize(std::string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

int main(int argc, char **argv) {
	if(argc != 4)
	{
		std::cerr << "Not enough arguments given, expected 3 got " << (argc - 1) << "." << std::endl;
		std::cerr << "Usages: [input file][output file][rate in lines/s]" << std::endl;
		return EXIT_FAILURE;
	}

	//Open inputstream and read everything as binary.
	std::ifstream testDataInputStream(argv[1], std::ios::in | std::ios::binary);
	//Open outputstream which discards old information and outputs as binary.
	std::ofstream testDataOutputStream(argv[2], std::ios::out | std::ios::trunc | std::ios::binary);
	std::size_t sleepAmount = (1.0 / std::atof(argv[3])) * 1000000000;

	//Can we open our input and output file.
	if(!testDataInputStream)
	{
		std::cerr << "Failed to open input file '" << argv[1] << "'" << std::endl;
		return EXIT_FAILURE;
	}
	if(!testDataOutputStream)
	{
		std::cerr << "Failed to open output stream" << std::endl;
		return EXIT_FAILURE;
	}

    std::cout << "system_clock" << std::endl;
    std::cout << std::chrono::system_clock::period::num << std::endl;
    std::cout << std::chrono::system_clock::period::den << std::endl;
    std::cout << "steady = " << std::boolalpha << std::chrono::system_clock::is_steady << std::endl << std::endl;

    std::cout << "high_resolution_clock" << std::endl;
    std::cout << std::chrono::high_resolution_clock::period::num << std::endl;
    std::cout << std::chrono::high_resolution_clock::period::den << std::endl;
    std::cout << "steady = " << std::boolalpha << std::chrono::high_resolution_clock::is_steady << std::endl << std::endl;

    std::cout << "steady_clock" << std::endl;
    std::cout << std::chrono::steady_clock::period::num << std::endl;
    std::cout << std::chrono::steady_clock::period::den << std::endl;
    std::cout << "steady = " << std::boolalpha << std::chrono::steady_clock::is_steady << std::endl << std::endl;

	std::size_t fileSize = getFileSize(argv[1]);

	std::cout << std::chrono::high_resolution_clock::duration::period::den << std::endl;

	std::size_t zeroTimeDifferenceAmount = 0;
	std::chrono::time_point<std::chrono::steady_clock> subStart;
	std::chrono::time_point<std::chrono::steady_clock> subEnd;
	std::chrono::duration<int64_t,std::chrono::_V2::steady_clock::period>::rep subDuration;
	char * buffer = new char [BYTESCHUNCKTOREAD];
	auto start = std::chrono::steady_clock::now();

	while(!testDataInputStream.eof())
	{
		subStart = std::chrono::steady_clock::now();
		testDataInputStream.read(buffer, BYTESCHUNCKTOREAD);

		//Is it the last byte of the input.
		if(testDataInputStream.eof())
		{
			auto end = std::chrono::steady_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
			double inSeconds = (duration - sleepAmount) / 1000000000.0;
			double linesRead = ((fileSize - 1.0) / (BYTESCHUNCKTOREAD + 3));
			std::cout << std::endl;
			std::cout << "Read " << linesRead << " lines" << std::endl;
			std::cout << "in " << inSeconds << " seconds" << std::endl;
			std::cout << "Read input file with: " << 1.0 / inSeconds * linesRead << " lines per second" << std::endl;
			std::cout << "Deviation: " << (std::atof(argv[3]) - 1.0 / inSeconds * linesRead) / std::atof(argv[3]) * 100 << std::endl;
			std::cout << "Time difference of 0: " << zeroTimeDifferenceAmount << std::endl;
			std::cout << "Read input file with: " << 1.0 / inSeconds * linesRead * (BYTESCHUNCKTOREAD + 3) << " bytes per seconden" << std::endl;
			std::cout <<  "DONE" << std::endl;
			break;
		}

		//Write to file and console.
		testDataOutputStream.write(buffer, BYTESCHUNCKTOREAD);
		testDataOutputStream << std::flush;

		std::cout << std::fixed << std::setprecision(10) << " at: " << testDataInputStream.tellg() << "/" << fileSize << "\r" << std::flush;
		subEnd = std::chrono::steady_clock::now();
		subDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(subEnd - subStart).count();
		if(subDuration <= 0)
		{
			zeroTimeDifferenceAmount++;
		}
		std::this_thread::sleep_for(std::chrono::nanoseconds(std::max(sleepAmount - subDuration, (long long unsigned)0)));
	}

	delete buffer;

	return EXIT_SUCCESS;
}
