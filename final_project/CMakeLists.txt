set(target final_project)
set( final_project_SOURCES
final_project.cpp
	)

add_executable( ${target} ${final_project_SOURCES} )

target_compile_definitions(${target}
		PRIVATE
		GLFW_INCLUDE_NONE
		)

target_link_libraries( ${target}
		PRIVATE
		ingredients
		glfw
		${OPENGL_gl_LIBRARY}
		)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shader DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/surfaceData.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
