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
	${MINIZIP_LIBRARIES}
	${PNG_LIBRARY}
)

