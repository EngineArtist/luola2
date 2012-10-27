include_directories(
	${Boost_INCLUDE_DIRS}
	${GLFW_INCLUDE_DIR}
)

file(
	GLOB_RECURSE SOURCES
	"*.cpp"
)

add_executable(
	luola2
	${SOURCES}
)

target_link_libraries(
	luola2
	${Boost_LIBRARIES}
	${GLFW_LIBRARIES}
	${GLEW_LIBRARY}
	${GLM_LIBRARY}
	${PNG_LIBRARY}
)

if (MINIZIP_FOUND)
    target_link_libraries(
        luola2
	    ${MINIZIP_LIBRARIES}
    )
endif (MINIZIP_FOUND)
