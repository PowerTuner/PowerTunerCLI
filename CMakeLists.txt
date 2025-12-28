cmake_minimum_required(VERSION 3.23)

project(PowerTunerCLI VERSION 1.1 DESCRIPTION "Official PowerTuner CLI Client" LANGUAGES CXX)

option(WITH_INTEL "Enable support for Intel CPUs" ON)
option(WITH_AMD "Enable support for AMD CPUs" ON)

set(PROJECT_AUTHOR "kylon")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(PRIV_DEFS "")

find_package(Qt6 6.10 REQUIRED COMPONENTS Core Network)

configure_file(src/Resources/version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/version.h)

add_subdirectory(src/external/libPWTShared)
add_subdirectory(src/external/libPWTClientService)
add_subdirectory(src/external/libPWTClientCommon)

qt_add_resources(QT_RESOURCE
    src/external/commonResources/commonResource.qrc
)

set(PROJECT_SOURCES
    src/Classes/CLISettings.h
    src/Classes/CLISettings.cpp
    src/Classes/FileLogger.h
    src/Classes/FileLogger.cpp

    src/CMDParser/SettingsArguments.h
    src/CMDParser/CMDArg.h
    src/CMDParser/CMDParser.h
    src/CMDParser/CMDParser.cpp

    src/Include/MessageType.h

    src/Commands/AppCommands.h
    src/Commands/AppCommands.cpp

    src/CliHelper/CliHelper.h
    src/CliHelper/OS/Linux/CliHelperLinux.h
    src/CliHelper/OS/Linux/CliHelperLinux.cpp
    src/CliHelper/OS/Windows/CliHelperWindows.h
    src/CliHelper/OS/Windows/CliHelperWindows.cpp
    src/CliHelper/Misc/CliHelperFan.h
    src/CliHelper/Misc/CliHelperFan.cpp

    src/Utils.h
    src/Utils.cpp
    src/PowerTunerCLI.h
    src/PowerTunerCLI.cpp
    src/main.cpp
)

if (WITH_INTEL)
    message(STATUS "${PROJECT_NAME}: Intel CPU support is enabled")
    list(APPEND PRIV_DEFS WITH_INTEL)

    list(APPEND PROJECT_SOURCES
        src/CliHelper/Vendor/Intel/CliHelperIntel.h
        src/CliHelper/Vendor/Intel/CliHelperIntel.cpp
    )
endif ()

if (WITH_AMD)
    message(STATUS "${PROJECT_NAME}: AMD CPU support is enabled")
    list(APPEND PRIV_DEFS WITH_AMD)

    list(APPEND PROJECT_SOURCES
        src/CliHelper/Vendor/AMD/CliHelperAMD.h
        src/CliHelper/Vendor/AMD/CliHelperAMD.cpp
        src/CliHelper/OS/Linux/CliHelperLinuxAMD.h
        src/CliHelper/OS/Linux/CliHelperLinuxAMD.cpp
    )
endif ()

if (WIN32)
    add_subdirectory(src/external/libPWTWin32)
    configure_file(src/Resources/Windows/win.rc.in ${CMAKE_CURRENT_SOURCE_DIR}/win.rc)

    list(APPEND PROJECT_SOURCES
        win.rc
    )
endif ()

qt_add_executable(${PROJECT_NAME} ${QT_RESOURCE} ${PROJECT_SOURCES})

include(CheckIPOSupported)
check_ipo_supported(RESULT has_ipo OUTPUT ipo_error)
if (has_ipo)
    message(STATUS "${PROJECT_NAME}: IPO/LTO enabled")
    set_property(TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif ()

target_compile_definitions(${PROJECT_NAME} PRIVATE ${PRIV_DEFS})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt::Core Qt::Network PWT::ClientCommon PWT::Shared PWT::ClientService)

install(TARGETS ${PROJECT_NAME}
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET ${PROJECT_NAME}
    OUTPUT_SCRIPT deploy_script
    NO_TRANSLATIONS
	NO_COMPILER_RUNTIME
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
