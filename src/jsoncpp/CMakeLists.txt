# 子目录下的cmakelists.txt文件 
#
# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量
aux_source_directory(. DIR_LIB_SRCS)

# 生成(静态)链接库
add_library (jsoncpp ${DIR_LIB_SRCS})



# if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.1.2)
#     #-Werror=* was introduced -after- GCC 4.1.2
#     add_compile_options("-Werror=strict-aliasing")
# endif()

# include(CheckIncludeFileCXX)
# include(CheckTypeSize)
# include(CheckStructHasMember)
# include(CheckCXXSymbolExists)

# check_include_file_cxx(clocale HAVE_CLOCALE)
# check_cxx_symbol_exists(localeconv clocale HAVE_LOCALECONV)

# set(CMAKE_EXTRA_INCLUDE_FILES clocale)
# check_type_size(lconv LCONV_SIZE LANGUAGE CXX)
# unset(CMAKE_EXTRA_INCLUDE_FILES)
# check_struct_has_member(lconv decimal_point clocale HAVE_DECIMAL_POINT LANGUAGE CXX)

# if(NOT (HAVE_CLOCALE AND HAVE_LCONV_SIZE AND HAVE_DECIMAL_POINT AND HAVE_LOCALECONV))
#     message(WARNING "Locale functionality is not supported")
#     if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.12.0)
#         add_compile_definitions(JSONCPP_NO_LOCALE_SUPPORT)
#     else()
#         add_definitions(-DJSONCPP_NO_LOCALE_SUPPORT)
#     endif()
# endif()

# set(JSONCPP_INCLUDE_DIR ../../include)

# set(PUBLIC_HEADERS
#     ${JSONCPP_INCLUDE_DIR}/json/config.h
#     ${JSONCPP_INCLUDE_DIR}/json/forwards.h
#     ${JSONCPP_INCLUDE_DIR}/json/json_features.h
#     ${JSONCPP_INCLUDE_DIR}/json/value.h
#     ${JSONCPP_INCLUDE_DIR}/json/reader.h
#     ${JSONCPP_INCLUDE_DIR}/json/version.h
#     ${JSONCPP_INCLUDE_DIR}/json/writer.h
#     ${JSONCPP_INCLUDE_DIR}/json/assertions.h
# )

# source_group("Public API" FILES ${PUBLIC_HEADERS})

# set(JSONCPP_SOURCES
#     json_tool.h
#     json_reader.cpp
#     json_valueiterator.inl
#     json_value.cpp
#     json_writer.cpp
# )

# # Install instructions for this target
# if(JSONCPP_WITH_CMAKE_PACKAGE)
#     set(INSTALL_EXPORT EXPORT jsoncpp)
# else()
#     set(INSTALL_EXPORT)
# endif()

# # Specify compiler features required when compiling a given target.
# # See https://cmake.org/cmake/help/v3.1/prop_gbl/CMAKE_CXX_KNOWN_FEATURES.html#prop_gbl:CMAKE_CXX_KNOWN_FEATURES
# # for complete list of features available
# list(APPEND REQUIRED_FEATURES
#         cxx_std_11 # Compiler mode is aware of C++ 11.
#         #MSVC 1900 cxx_alignas # Alignment control alignas, as defined in N2341.
#         #MSVC 1900 cxx_alignof # Alignment control alignof, as defined in N2341.
#         #MSVC 1900 cxx_attributes # Generic attributes, as defined in N2761.
#         cxx_auto_type # Automatic type deduction, as defined in N1984.
#         #MSVC 1900 cxx_constexpr # Constant expressions, as defined in N2235.
#         cxx_decltype # Decltype, as defined in N2343.
#         cxx_default_function_template_args # Default template arguments for function templates, as defined in DR226
#         cxx_defaulted_functions # Defaulted functions, as defined in N2346.
#         #MSVC 1900 cxx_defaulted_move_initializers # Defaulted move initializers, as defined in N3053.
#         cxx_delegating_constructors # Delegating constructors, as defined in N1986.
#         #MSVC 1900 cxx_deleted_functions # Deleted functions, as defined in N2346.
#         cxx_enum_forward_declarations # Enum forward declarations, as defined in N2764.
#         cxx_explicit_conversions # Explicit conversion operators, as defined in N2437.
#         cxx_extended_friend_declarations # Extended friend declarations, as defined in N1791.
#         cxx_extern_templates # Extern templates, as defined in N1987.
#         cxx_final # Override control final keyword, as defined in N2928, N3206 and N3272.
#         #MSVC 1900 cxx_func_identifier # Predefined __func__ identifier, as defined in N2340.
#         #MSVC 1900 cxx_generalized_initializers # Initializer lists, as defined in N2672.
#         #MSVC 1900 cxx_inheriting_constructors # Inheriting constructors, as defined in N2540.
#         #MSVC 1900 cxx_inline_namespaces # Inline namespaces, as defined in N2535.
#         cxx_lambdas # Lambda functions, as defined in N2927.
#         #MSVC 1900 cxx_local_type_template_args # Local and unnamed types as template arguments, as defined in N2657.
#         cxx_long_long_type # long long type, as defined in N1811.
#         #MSVC 1900 cxx_noexcept # Exception specifications, as defined in N3050.
#         #MSVC 1900 cxx_nonstatic_member_init # Non-static data member initialization, as defined in N2756.
#         cxx_nullptr # Null pointer, as defined in N2431.
#         cxx_override # Override control override keyword, as defined in N2928, N3206 and N3272.
#         cxx_range_for # Range-based for, as defined in N2930.
#         cxx_raw_string_literals # Raw string literals, as defined in N2442.
#         #MSVC 1900 cxx_reference_qualified_functions # Reference qualified functions, as defined in N2439.
#         cxx_right_angle_brackets # Right angle bracket parsing, as defined in N1757.
#         cxx_rvalue_references # R-value references, as defined in N2118.
#         #MSVC 1900 cxx_sizeof_member # Size of non-static data members, as defined in N2253.
#         cxx_static_assert # Static assert, as defined in N1720.
#         cxx_strong_enums # Strongly typed enums, as defined in N2347.
#         #MSVC 1900 cxx_thread_local # Thread-local variables, as defined in N2659.
#         cxx_trailing_return_types # Automatic function return type, as defined in N2541.
#         #MSVC 1900 cxx_unicode_literals # Unicode string literals, as defined in N2442.
#         cxx_uniform_initialization # Uniform initialization, as defined in N2640.
#         #MSVC 1900 cxx_unrestricted_unions # Unrestricted unions, as defined in N2544.
#         #MSVC 1900 cxx_user_literals # User-defined literals, as defined in N2765.
#         cxx_variadic_macros # Variadic macros, as defined in N1653.
#         cxx_variadic_templates # Variadic templates, as defined in N2242.
# )


# if(BUILD_SHARED_LIBS)
#     if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.12.0)
#         add_compile_definitions(JSON_DLL_BUILD)
#     else()
#         add_definitions(-DJSON_DLL_BUILD)
#     endif()

#     set(SHARED_LIB ${PROJECT_NAME}_lib)
#     add_library(${SHARED_LIB} SHARED ${PUBLIC_HEADERS} ${JSONCPP_SOURCES})
#     set_target_properties(${SHARED_LIB} PROPERTIES
#         OUTPUT_NAME jsoncpp
#         VERSION ${PROJECT_VERSION}
#         SOVERSION ${PROJECT_SOVERSION}
#         POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS}
#     )

#     # Set library's runtime search path on OSX
#     if(APPLE)
#         set_target_properties(${SHARED_LIB} PROPERTIES INSTALL_RPATH "@loader_path/.")
#     endif()

#     target_compile_features(${SHARED_LIB} PUBLIC ${REQUIRED_FEATURES})

#     target_include_directories(${SHARED_LIB} PUBLIC
#         $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
#         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/${JSONCPP_INCLUDE_DIR}>
#         $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include/json>
#     )

#     list(APPEND CMAKE_TARGETS ${SHARED_LIB})
# endif()

# if(BUILD_STATIC_LIBS)
#     set(STATIC_LIB ${PROJECT_NAME}_static)
#     add_library(${STATIC_LIB} STATIC ${PUBLIC_HEADERS} ${JSONCPP_SOURCES})

#     # avoid name clashes on windows as the shared import lib is also named jsoncpp.lib
#     if(NOT DEFINED STATIC_SUFFIX AND BUILD_SHARED_LIBS)
#         if (MSVC)
#             set(STATIC_SUFFIX "_static")
#         else()
#             set(STATIC_SUFFIX "")
#         endif()
#     endif()

#     set_target_properties(${STATIC_LIB} PROPERTIES
#         OUTPUT_NAME jsoncpp${STATIC_SUFFIX}
#         VERSION ${PROJECT_VERSION}
#     )

#     # Set library's runtime search path on OSX
#     if(APPLE)
#         set_target_properties(${STATIC_LIB} PROPERTIES INSTALL_RPATH "@loader_path/.")
#     endif()

#     target_compile_features(${STATIC_LIB} PUBLIC ${REQUIRED_FEATURES})

#     target_include_directories(${STATIC_LIB} PUBLIC
#         $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
#         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/${JSONCPP_INCLUDE_DIR}>
#         $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include/json>
#     )

#     list(APPEND CMAKE_TARGETS ${STATIC_LIB})
# endif()

# if(BUILD_OBJECT_LIBS)
#     set(OBJECT_LIB ${PROJECT_NAME}_object)
#     add_library(${OBJECT_LIB} OBJECT ${PUBLIC_HEADERS} ${JSONCPP_SOURCES})

#     set_target_properties(${OBJECT_LIB} PROPERTIES
#         OUTPUT_NAME jsoncpp
#         VERSION ${PROJECT_VERSION}
#         SOVERSION ${PROJECT_SOVERSION}
#         POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS}
#     )

#     # Set library's runtime search path on OSX
#     if(APPLE)
#         set_target_properties(${OBJECT_LIB} PROPERTIES INSTALL_RPATH "@loader_path/.")
#     endif()

#     target_compile_features(${OBJECT_LIB} PUBLIC ${REQUIRED_FEATURES})

#     target_include_directories(${OBJECT_LIB} PUBLIC
#         $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
#         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/${JSONCPP_INCLUDE_DIR}>
#         $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include/json>
#     )

#     list(APPEND CMAKE_TARGETS ${OBJECT_LIB})
# endif()

# install(TARGETS ${CMAKE_TARGETS} ${INSTALL_EXPORT}
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     OBJECTS DESTINATION ${CMAKE_INSTALL_LIBDIR}
# )

