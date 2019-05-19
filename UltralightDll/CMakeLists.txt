set(APP_NAME UltralightDll)
include_directories(${UltralightSDKIncludes})
link_directories(${UltralightSDKLibs})
link_libraries(UltralightCore Ultralight WebCore)

add_library(${APP_NAME} SHARED "main.cpp" "FileSystemBasic.cpp" "FileSystemBasic.h" )

# Copy all binaries to target directory
add_custom_command(TARGET ${APP_NAME} POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory ${UltralightSDKBinaries} $<TARGET_FILE_DIR:${APP_NAME}>) 