# Logger

A simple C++ logging setup that builds on [spdlog](https://github.com/gabime/spdlog) to provide easily configurable console and file outputs, along with convenient logging macros.

## Features
- **Console** and **File** output sinks.
- Configurable log levels, file sizes, and rotation behavior.
- Log macros (`LOG_INFO`, `LOG_ERROR`, etc.) with `fmt` formatting.
- Simple [CMake](https://cmake.org/) build and example usage.
- **JSON configuration initialization**: Easily set up logger sinks via a JSON config file


## Usage Example

### In your CMakeLists.txt:
#### Add Logger to Your Project
You can include this Logger project as a subdirectory in your own CMake project. For example, if you clone or include it as a git submodule under submodules/Logger, you can do:

#### In your top-level CMakeLists.txt
```cmake
add_subdirectory(submodules/Logger)
```

#### Link Logger to Your Executable/Library
Next, link the Logger library to your target. For instance:

```cmake
target_link_libraries(MyApp PRIVATE Logger)
```

Because the Logger uses a PUBLIC include for its headers, you do not need extra include_directories for the Logger — they are automatically made available to your target.


### In your code:

### Programmatic Initialization

You can manually add sinks using the provided builder methods:

```cpp
#include "Logger.h"

int main()
{
    // Setup file output with custom options
    logging::addFileOutput()
        .setFilename("TestLog.log")
        .setLevel(LogLevel::Info)
        .setMaxFileSize(20_MB)
        .setRotateOnSession(true);

    // Setup console output with custom level
    logging::addConsoleOutput().setLevel(LogLevel::Debug);

    LOG_INFO("Hello logger, the answer is: {}", 42);

    return 0;
}
```

### JSON Configuration Initialization

Alternatively, you can initialize the logger via a JSON configuration file. Create a JSON file (e.g. `logger_config.json`) with the following structure:

```json
{
    "sinks": [
        {
            "type": "console",
            "level": "debug",
            "max_skip_duration": 0,
            "pattern": "[%Y-%m-%d %H:%M:%S.%e] [%l] %v"
        },
        {
            "type": "file",
            "level": "info",
            "max_skip_duration": 0,
            "file_name": "logs/app.log",
            "max_file_size": "10_MB",
            "max_files": 3,
            "rotate_on_session": true
        }
    ]
}
```

Then in your code initialize the logger using:

```cpp
#include "Logger.h"

int main()
{
    // Initialize logger with JSON configuration.
    // Missing settings will be replaced with default values.
    logging::initializeLogger("logger_config.json");

    LOG_INFO("Logger initialized using JSON configuration!");

    return 0;
}

```

This approach leverages [nlohmann json](https://github.com/nlohmann/json) (integrated via CPM) to parse the configuration file and set up the sinks accordingly. If a key is missing, a sensible default will be used.

### Overriding JSON Configuration Keys

This project defines the JSON key names (used in the configuration file) as CMake Cache variables. This means that when you include Logger as a subdirectory in your project, you can easily override these defaults without modifying the Logger source code.

### How to override

Before adding the Logger subdirectory in your parent project's CMakeLists.txt, set the desired values for the JSON keys. For example:

```cmake
# Override default JSON key names
set(LOGGER_CONFIG_SINK "custom_sinks" CACHE STRING "JSON key for logger sinks")
set(LOGGER_CONFIG_LEVEL "custom_level" CACHE STRING "JSON key for log level")
# ...set other keys as needed

add_subdirectory(path/to/Logger)
```

Alternatively, you can override these values on the command line when configuring your project:

```bash
cmake -DLOGGER_CONFIG_SINK="custom_sinks" -DLOGGER_CONFIG_LEVEL="custom_level" <path-to-your-project>
```


## Default values

If not otherwise specified, the logger will provide default values:

### Console Sink Defaults
- **Log Level** : `info`
- **Max Skip Duration** : `0` (microseconds)
- **Pattern** : `[%Y-%m-%d %H:%M:%S.%e] [%l] %v`

### File Sink Defaults
- **Log Level** : `info`
- **Max Skip Duration** : `0` (microseconds)
- **File Name** : `default.log`
- **Max File Size** : `10 MB`
- **Max Files** : `3`
- **Rotate on session** : `false`

### MSVC Sink Defaults (Windows Only)
- **Log Level** : `info`
- **Max Skip Duration** : `0` (microseconds)
- **Check For Debugger** : `false`


## Notes
- Update `CMakeLists.txt` or `build.py` according to your needs.
- Requires **C++20** and **CMake ≥ 3.25**.
- By default, logs are written at *Info* level to the configured sinks.  
- The [spdlog](https://github.com/gabime/spdlog) library is included via CPM in the `libs` directory. 

Enjoy streamlined logging with SPDLog!
