#ifndef LPT_LOG_OUTPUT_H
#define LPT_LOG_OUTPUT_H
#include <iostream>
#include <fstream>
#include <string>
#include "FileManager.h"

namespace LPT
{
  //! @brief メッセージ出力を行なうクラス
  class LPT_LOG
  {
    std::ofstream logfile;
    const bool FileFlushFlag;
  private:
    //Singletonパターンを適用
    LPT_LOG():
#if defined(LPT_DEBUG) || defined(LPT_DEBUG_ENABLE)
      FileFlushFlag(true)
#else
      FileFlushFlag(false)
#endif
    {
      logfile.open((FileManager::GetInstance()->GetFileName("log")).c_str());
    }
    ~LPT_LOG()
    {
    }
    LPT_LOG(const FileManager & obj);
    LPT_LOG & operator=(const FileManager & obj);
  public:
    static LPT_LOG *GetInstance()
    {
      static LPT_LOG instance;
      return &instance;
    }
    void DEBUG(std::string message)
    {
#if defined(LPT_DEBUG_ENABLE) || defined(LPT_VERBOSE)
      logfile << "LPT DEBUG  : " << message << "\n";
      if(FileFlushFlag) logfile.flush();
#endif
    }
    template < typename T > void DEBUG(std::string message, T value)
    {
#if defined(LPT_DEBUG_ENABLE) || defined(LPT_VERBOSE)
      logfile << "LPT DEBUG  : " << message << value << "\n";
      if(FileFlushFlag) logfile.flush();
#endif
    }
    template < typename T > void DEBUG(std::string message, T * value, int max)
    {
#if defined(LPT_DEBUG_ENABLE) || defined(LPT_VERBOSE)
      logfile << "LPT DEBUG  : " << message;
      for(int i = 0; i < max - 1; i++) {
        logfile << value[i] << ",";
      }
      logfile << value[max - 1] << "\n";

      if(FileFlushFlag) logfile.flush();
#endif
    }

    void LOG(std::string message)
    {
#if defined(LPT_LOG_ENABLE) || defined(LPT_VERBOSE)
      logfile << "LPT LOG  : " << message << "\n";
      if(FileFlushFlag) logfile.flush();
#endif
    }
    template < typename T > void LOG(std::string message, T value)
    {
#if defined(LPT_LOG_ENABLE) || defined(LPT_VERBOSE)
      logfile << "LPT LOG  : " << message << value << "\n";
      if(FileFlushFlag) logfile.flush();
#endif
    }
    template < typename T > void LOG(std::string message, T * value, int max)
    {
#if defined(LPT_LOG_ENABLE) || defined(LPT_VERBOSE)
      logfile << "LPT LOG  : " << message;
      for(int i = 0; i < max - 1; i++) {
        logfile << value[i] << ",";
      }
      logfile << value[max - 1] << "\n";

      if(FileFlushFlag) logfile.flush();
#endif
    }

    void INFO(std::string message)
    {
      logfile << "LPT INFO : " << message << "\n";
      if(FileFlushFlag) logfile.flush();
    }

    template < typename T > void INFO(std::string message, T value)
    {
      logfile << "LPT INFO : " << message << value << "\n";
      if(FileFlushFlag) logfile.flush();
    }
    template < typename T > void INFO(std::string message, T * value, int max)
    {
      logfile << "LPT INFO : " << message;
      for(int i = 0; i < max - 1; i++) {
        logfile << value[i] << ",";
      }
      logfile << value[max - 1] << "\n";
      if(FileFlushFlag) logfile.flush();
    }

    void WARN(std::string message)
    {
      logfile << "LPT WARN : " << message << "\n";
      if(FileFlushFlag) logfile.flush();
    }
    template < typename T > void WARN(std::string message, T value)
    {
      logfile << "LPT WARN : " << message << value << "\n";
      if(FileFlushFlag) logfile.flush();
    }

    void ERROR(std::string message)
    {
      logfile << "LPT ERROR: " << message << std::endl;
    }
    template < typename T > void ERROR(std::string message, T value)
    {
      logfile << "LPT ERROR : " << message << value << std::endl;
    }
  };
} // namespace LPT
#endif
