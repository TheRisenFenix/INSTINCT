/// @file ConfigManager.hpp
/// @brief Config management for the Project
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2020-03-11

#pragma once

#include <vector>
#include <deque>
#include <memory>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>

namespace NAV::ConfigManager
{
/// Map which stores all options
extern boost::program_options::variables_map vm;

/// @brief Initializes the config manager. Call this function before using other functions
void initialize();

/// @brief Deinitializes the config manager. Call this if you want to Fetch config again
void deinitialize();

/// @brief Get the Program Options object
/// @return The object
[[nodiscard]] const boost::program_options::options_description& GetProgramOptions();

/// @brief Fetches the configs from the command line parameters
/// @param[in, out] argc Number of command line parameters
/// @param[in, out] argv Array of the command line parameters
void FetchConfigs(const int argc, const char* argv[]); // NOLINT

/// @brief Retrieves the value of a corresponding key from the configuration, if one exists.
/// @tparam T Return value type
/// @param[in] key Key to search for
/// @param[in] defaultValue If key is not found, the default value is returned
/// @return The value found with the key or the default value
template<typename T>
const T& Get(const std::string& key, const T& defaultValue)
{
    if (vm.count(key))
    {
        return vm[key].as<T>();
    }

    return defaultValue;
}

/// Checks if a corresponding key exists in the configuration.
bool HasKey(const std::string& key);

/// Returns all keys in the configuration, as a vector.
std::vector<std::string> GetKeys();

} // namespace NAV::ConfigManager