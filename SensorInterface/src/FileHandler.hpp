/*
 * FileHandler.hpp
 *
 *  Created on: 2 Jul 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_FILEHANDLER_HPP_
#define SRC_FILEHANDLER_HPP_

#include <fstream>
#include <string>

/**
 * Convenience class for writing to files.
 */
class FileHandler
{
  public:
    FileHandler()
    {
    }

    explicit FileHandler(const std::string& fileName)
    {
        openFile(fileName);
    }

    virtual ~FileHandler()
    {
        output_stream_.close();
    }

    FileHandler(FileHandler const&) = delete;
    void operator=(FileHandler const&) = delete;

    /**
     * Creates a new file with a name and opens it.
     * @param fileName
     */
    void createNewFile(const std::string& fileName)
    {
        output_stream_.open(fileName, std::ios::trunc | std::ios::binary);
    }

    /**
     * Opens file in append mode.
     * @param fileName
     */
    void openFile(const std::string& fileName)
    {
        output_stream_.open(fileName, std::ios::app | std::ios::binary);
    }

    /**
     * Writes a string to a file.
     * @param data
     */
    void writeToFile(const std::string& data)
    {
        if (!output_stream_.is_open())
        {
            throw std::runtime_error("Outputstream hasn't been opened!");
        }
        output_stream_ << data;
    }

    /**
     * Gets a reference to the filestream if opened.
     * @return
     */
    std::ofstream& getOutputStream()
    {
        if (!output_stream_.is_open())
        {
            throw std::runtime_error("Outputstream hasn't been opened!");
        }
        return output_stream_;
    }

  private:
    std::ofstream output_stream_;
};
#endif /* SRC_FILEHANDLER_HPP_ */
