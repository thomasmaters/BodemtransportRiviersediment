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

namespace Controller
{
/*
 *
 */
class FileHandler
{
  public:
    FileHandler()
    {
    }

    FileHandler(const std::string& fileName)
    {
        openFile(fileName);
    }

    virtual ~FileHandler()
    {
        output_stream_.close();
    }

    FileHandler(FileHandler const&) = delete;
    void operator=(FileHandler const&) = delete;

    void createNewFile(const std::string& fileName)
    {
        output_stream_.open(fileName, std::ios::trunc | std::ios::binary);
    }

    void openFile(const std::string& fileName)
    {
        output_stream_.open(fileName, std::ios::app | std::ios::binary);
    }

    void writeToFile(const std::string& data)
    {
        if (!output_stream_.is_open())
        {
            throw std::runtime_error("Outputstream hasn't been opened!");
        }
        output_stream_ << data;
    }

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

} /* namespace Controller */

#endif /* SRC_FILEHANDLER_HPP_ */
