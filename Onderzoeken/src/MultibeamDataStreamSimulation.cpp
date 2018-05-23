#include <iostream>
#include <iomanip>
#include <fstream>
#include <thread>
#include <chrono>

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
	std::size_t sleepAmount = (1.0 / std::atof(argv[3])) * 1000000;

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

	std::size_t fileSize = getFileSize(argv[1]);

	char * buffer = new char [BYTESCHUNCKTOREAD];
	auto start = std::chrono::high_resolution_clock::now();
	while(!testDataInputStream.eof())
	{
		auto subStart = std::chrono::high_resolution_clock::now();
		testDataInputStream.read(buffer, BYTESCHUNCKTOREAD);

		//Is it the last byte of the input.
		if(testDataInputStream.eof())
		{
			auto end = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
			double inSeconds = (duration - sleepAmount) / 1000000.0;
			double linesRead = ((fileSize - 1.0) / (BYTESCHUNCKTOREAD + 3));
			std::cout << std::endl;
			std::cout << "Read " << linesRead << " lines" << std::endl;
			std::cout << "in " << inSeconds << " seconds" << std::endl;
			std::cout << "Read input file with: " << 1.0 / inSeconds * linesRead << " lines per second" << std::endl;
			std::cout << "Read input file with: " << 1.0 / inSeconds * linesRead * (BYTESCHUNCKTOREAD + 3) << " bytes per seconden" << std::endl;
			std::cout <<  "DONE" << std::endl;
			break;
		}

		//Write to file and console.
		testDataOutputStream.write(buffer, BYTESCHUNCKTOREAD);
		testDataOutputStream << std::flush;

		std::cout << std::fixed << std::setprecision(10) << " at: " << testDataInputStream.tellg() << "/" << fileSize << /*" executiontime: "
				<< subDuration << " sleeptime: " << std::setw(10) << sleepAmount - subDuration << std::setw(20) << */ "\r" << std::flush;
		auto subEnd = std::chrono::high_resolution_clock::now();
		auto subDuration = std::chrono::duration_cast<std::chrono::microseconds>(subEnd - subStart).count();

		std::this_thread::sleep_for(std::chrono::microseconds(std::max(sleepAmount - subDuration, (long long unsigned)0)));
	}

	delete buffer;

	return EXIT_SUCCESS;
}
