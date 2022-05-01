cmake_minimum_required(VERSION 3.5)

project(passman++-cli LANGUAGES CXX VERSION 1.0.0)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)

set(BIN_INSTALL_DIR "${CMAKE_INSTALL_BINDIR}")
set(DATA_INSTALL_DIR "${CMAKE_INSTALL_DATAROOTDIR}")

add_executable(passman++
        main.cpp
        util.cpp

        commands/mkdb.cpp
        commands/info.cpp
        commands/edit.cpp
        commands/mkentry.cpp
        commands/rm.cpp

        commands/command.cpp

        #../libpassman/src/data_stream.cpp
        #../libpassman/src/extra.cpp
        #../libpassman/src/field.cpp
        #../libpassman/src/kdf.cpp
        #../libpassman/src/pdpp_database.cpp
        #../libpassman/src/pdpp_entry.cpp
        #../libpassman/src/vector_union.cpp
        #../libpassman/src/2fa.cpp
)

find_package(Qt6 COMPONENTS Core Sql REQUIRED)

include(FindPkgConfig)

pkg_check_modules(PASSMAN REQUIRED passman)
pkg_check_modules(BOTAN2 REQUIRED botan-2)

target_include_directories(passman++ PUBLIC /usr/include/botan-2)
#target_include_directories(passman++ PRIVATE ../libpassman/include)

target_link_libraries(passman++ PRIVATE
        Qt::Core
        Qt::Sql
        botan-2
        passman
)

target_compile_options(passman++ PUBLIC -O2 -march=native -Wall -Wextra -Wshadow -Wnon-virtual-dtor -Wunused -Wold-style-cast -Wconversion -Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches -pedantic -Wuseless-cast)

install(TARGETS passman++
    BUNDLE DESTINATION . COMPONENT Runtime
    RUNTIME DESTINATION ${BIN_INSTALL_DIR} COMPONENT Runtime)

#add_subdirectory(share)
