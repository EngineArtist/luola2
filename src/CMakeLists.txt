include_directories(
	${Boost_INCLUDE_DIRS}
	${GLFW_INCLUDE_PATH}
)

file(
	GLOB_RECURSE SOURCES
	"*.cpp"
)

add_executable(
	luola2
	${SOURCES}
)

set(CMAKE_EXE_LINKER_FLAGS
    "-framework OpenGL"
)

target_link_libraries(
	luola2
	${Boost_LIBRARIES}
	${GLFW_LIBRARY}
	${GLEW_LIBRARY}
	${GLM_LIBRARY}
	${PNG_LIBRARY}
	${YAMLCPP_LIBRARY}
)

if (MINIZIP_FOUND)
    target_link_libraries(
        luola2
	    ${MINIZIP_LIBRARIES}
    )
endif (MINIZIP_FOUND)
