/*
 * FileHandler.hpp
 *
 *  Created on: 2 Jul 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_FILEHANDLER_HPP_
#define SRC_FILEHANDLER_HPP_

#include <string>

namespace Controller
{
/*
 *
 */
class FileHandler
{
  public:
    static FileHandler& getInstance()
    {
        static FileHandler instance;
        return instance;
    }

    FileHandler(FileHandler const&) = delete;
    void operator=(FileHandler const&) = delete;

    void createNewFile(const std::string& fileName);
    void openFile(const std::string& path);
    void appendToFile(const std::string& data);

  private:
    FileHandler();
    virtual ~FileHandler();
};

} /* namespace Controller */

#endif /* SRC_FILEHANDLER_HPP_ */
