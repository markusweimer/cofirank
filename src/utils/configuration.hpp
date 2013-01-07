#ifndef _CONFIGURATION_HPP_
#define _CONFIGURATION_HPP_

#include <map>
#include <string>
#include <cassert>
#include "configexception.hpp"

/**
 * Simple class to hold program configuration.
 *
 * The class is implemented as a singleton. To get a reference the current instance, call
 * Configuration::getConfiguration()
 */
class Configuration{
public:
  
  /**
   * @return a reference to the current instance.
   */
  static Configuration& getInstance(void);
  
  /**
   * Set a double value in the configuration
   *
   * @param name the name of the configuration option.
   * @param value the value to be set.
   */
  void setDouble(std::string name, double value);
  
  /**
   * Get a double value from the configuration
   *
   * @param name the name of the configuration option.
   * @throws ConfigException if the given parameter cannot be found
   */	
  double getDouble(std::string name) throw (cofi::ConfigException);
  
  
  /**
   * Set a int value in the configuration
   *
   * @param name the name of the configuration option.
   * @param value the value to be set.
   */
  void setInt(std::string name, int value);
  
  /**
   * Get a int value from the configuration
   *
   * @param name the name of the configuration option.
   * @throws ConfigException if the given parameter cannot be found
   */	
  int getInt(std::string name) throw (cofi::ConfigException);
  
  /**
   * @return true if getInt(name) == 1, false otherwise.
   */ 
  bool getIntAsBool(std::string name) throw (cofi::ConfigException);
  /**
   * Set a string value in the configuration
   *
   * @param name the name of the configuration option.
   * @param value the value to be set.
   */
  void setString(std::string name, std::string value);


  /**
   * Get an int value from the configuration
   *
   * @param name the name of the configuration option.
   * @throws ConfigException if the given parameter cannot be found
   */	
  std::string getString(std::string name) throw (cofi::ConfigException);
  
  
  bool isSet(std::string name);
  /**
   * reads the configuration from file.
   *
   * The file format is very simple, each line holds one value. The lines have
   * the following format:
   *
   * [TYPE] [NAME] [VALUE]
   *
   * where [TYPE] can be: double, int or string
   *
   * Examples:
   *
   * int bmrm.maxIter 90
   * double bmrm.lambda 1.0
   * string logHeader This log was generated today. 
   *
   * @param fileName the name of the file to read.
   */
  
  void readFromFile(std::string fileName);
  
  /**
   * Writes the config to the given stream.
   * 
   * This method uses the same format as described in readFromFile()
   *
   * @param out the stream where the configuration will be written to
   */
  void write(std::ostream& out);
  
  /**
   * Writes the configuration to a file.
   *
   * This is a convenience wrapper around write()
   *
   * @param fileName the name of the file where the config will be stored.
   * @see write()
   */
  void writeToFile(std::string fileName);
  
  
  /**
   * Prints the usage statistics for all known fields to the given ostream.
   *
   * Very useful for debugging.
   * 
   * @param out the stream to write to.
   */
  void writeUsageStatistics(std::ostream& out);
  
  
private:
  
  void increaseReadCount(const std::string& name);
  void increaseWriteCount(const std::string& name);
  
  
  Configuration(void){}
  Configuration(const Configuration& other){assert(false);}

  std::map<std::string, double> doubles;
  std::map<std::string, int> ints;
  std::map<std::string, std::string> strings;
  
  std::map<std::string, unsigned int> readCount;
  std::map<std::string, unsigned int> writeCount;
  
  
  static Configuration* instance;
};


#endif /* _CONFIGURATION_HPP_ */
