if(WIN32)
    set(WINDOWS_BUILD TRUE CACHE INTERNAL "Windows Build")
elseif(APPLE)
    set(MACOS_BUILD TRUE CACHE INTERNAL "MacOS Build")
elseif(UNIX)
    set(LINUX_BUILD TRUE CACHE INTERNAL "Linux Build")
else()
    message(FATAL_ERROR "Cannot detect platform")
endif()

# --

function(set_xcode_global_property property value)
    if(MACOS_BUILD)
        set(CMAKE_XCODE_ATTRIBUTE_${property} ${value} CACHE INTERNAL "${ARGN}")
    endif()
endfunction()

# --

function(set_xcode_property target property value)
    if(MACOS_BUILD)
        set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_${property} ${value})
    endif()
endfunction()

# --

function(set_info_plist_property target property value)
    if(MACOS_BUILD)
        set_target_properties(${target} PROPERTIES MACOSX_BUNDLE_${property} ${value})
    endif()
endfunction()

# --

macro(has_flag var flag result)
    string(REGEX MATCH "${flag}" flag_found "${${var}}")
    if(flag_found)
        set(${result} TRUE)
    else()
        set(${result} FALSE)
    endif()
endmacro()

# --

macro(add_flag var flag)
    has_flag(${var} ${flag} found)
    if(NOT found)
        if(${var})
            set(${var} "${${var}} ${flag}")
        else()
            set(${var} "${flag}")
        endif()
    endif()
    unset(found)
endmacro()

# --

macro(add_c_cxx_flags)
    set(c_flags ${CMAKE_C_FLAGS})
    set(cxx_flags ${CMAKE_CXX_FLAGS})

    foreach(flag ${ARGN})
        add_flag(c_flags ${flag})
        add_flag(cxx_flags ${flag})
    endforeach()

    set(CMAKE_C_FLAGS "${c_flags}" CACHE STRING "Flags used by the C compiler during all build types." FORCE)
    set(CMAKE_CXX_FLAGS "${cxx_flags}" CACHE STRING "Flags used by the CXX compiler during all build types." FORCE)    
endmacro()

# --

macro(add_c_flags)
    set(c_flags ${CMAKE_C_FLAGS})

    foreach(flag ${ARGN})
        add_flag(c_flags ${flag})
    endforeach()

    set(CMAKE_C_FLAGS "${c_flags}" CACHE STRING "Flags used by the C compiler during all build types." FORCE)
endmacro()

# --

macro(add_cxx_flags)
    set(cxx_flags ${CMAKE_CXX_FLAGS})

    foreach(flag ${ARGN})
        add_flag(cxx_flags ${flag})
    endforeach()

    set(CMAKE_CXX_FLAGS "${cxx_flags}" CACHE STRING "Flags used by the CXX compiler during all build types." FORCE)    
endmacro()

# --

macro(add_c_cxx_flags_config config)
    set(c_flags ${CMAKE_C_FLAGS_${config}})
    set(cxx_flags ${CMAKE_CXX_FLAGS_${config}})

    foreach(flag ${ARGN})
        add_flag(c_flags ${flag})
        add_flag(cxx_flags ${flag})
    endforeach()

    set(CMAKE_C_FLAGS_${config} "${c_flags}" CACHE STRING "Flags used by the C compiler during ${config} builds." FORCE)
    set(CMAKE_CXX_FLAGS_${config} "${cxx_flags}" CACHE STRING "Flags used by the CXX compiler during ${config} builds." FORCE)
endmacro()

# --

macro(builder_configure_platform)
    set(CMAKE_CXX_STANDARD          17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS        ON)

    if(WINDOWS_BUILD)
        add_c_cxx_flags(/MP)

        # https://docs.microsoft.com/en-us/cpp/build/reference/zc-cplusplus
        add_cxx_flags(/Zc:__cplusplus)

        add_c_cxx_flags_config(DEBUG            /DDEBUG=1  /D_DEBUG=1  /D_HAS_ITERATOR_DEBUGGING=1 /D_SECURE_SCL=1)
        add_c_cxx_flags_config(RELEASE          /DNDEBUG=1 /D_NDEBUG=1 /D_HAS_ITERATOR_DEBUGGING=0 /D_SECURE_SCL=0)
        add_c_cxx_flags_config(MINSIZEREL       /DNDEBUG=1 /D_NDEBUG=1 /D_HAS_ITERATOR_DEBUGGING=0 /D_SECURE_SCL=0)
        add_c_cxx_flags_config(RELWITHDEBINFO   /DNDEBUG=1 /D_NDEBUG=1 /D_HAS_ITERATOR_DEBUGGING=0 /D_SECURE_SCL=0)
        
        add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -DUNICODE -D_UNICODE)
    elseif(MACOS_BUILD)
        add_c_cxx_flags(-ffunction-sections -fdata-sections -fno-constant-cfstrings -Wsign-compare -Wpointer-arith -Wshadow)        

        add_cxx_flags(-fno-threadsafe-statics)
        
        add_c_cxx_flags_config(DEBUG            -DDEBUG=1   -D_DEBUG=1)
        add_c_cxx_flags_config(RELEASE          -DNDEBUG=1  -D_NDEBUG=1)
        add_c_cxx_flags_config(MINSIZEREL       -DNDEBUG=1  -D_NDEBUG=1)
        add_c_cxx_flags_config(RELWITHDEBINFO   -DNDEBUG=1  -D_NDEBUG=1)
        
        add_definitions(-D__ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES=0)
        
        set_xcode_global_property(CLANG_CXX_LANGUAGE_STANDARD   c++17)
        set_xcode_global_property(CLANG_CXX_LIBRARY             libc++)
        set_xcode_global_property(CLANG_LINK_OBJC_RUNTIME       NO)
        
        set(CMAKE_MACOSX_RPATH                  FALSE)
        set(CMAKE_SKIP_BUILD_RPATH              FALSE)
        set(CMAKE_BUILD_WITH_INSTALL_RPATH      FALSE)
        set(CMAKE_INSTALL_RPATH_USE_LINK_PATH   FALSE)
        set(CMAKE_INSTALL_RPATH                 "")
    elseif(LINUX_BUILD)
        add_c_cxx_flags(-ffunction-sections -fdata-sections -Wsign-compare -Wpointer-arith -Wshadow)        
    
        add_cxx_flags(-fno-threadsafe-statics)
            
        add_c_cxx_flags_config(DEBUG            -DDEBUG=1   -D_DEBUG=1)
        add_c_cxx_flags_config(RELEASE          -DNDEBUG=1  -D_NDEBUG=1)
        add_c_cxx_flags_config(MINSIZEREL       -DNDEBUG=1  -D_NDEBUG=1)
        add_c_cxx_flags_config(RELWITHDEBINFO   -DNDEBUG=1  -D_NDEBUG=1)
    endif()
 endmacro()
 