cmake_minimum_required(VERSION 3.5)
project(mmpengine_basic_samples VERSION 1.0.0 LANGUAGES C CXX)

add_subdirectory(libs/mmpengine)

function(addSample appName)

	add_executable(${appName}
		src/samples/${appName}/Main.cpp
	)

	get_property(core_name_tmp GLOBAL PROPERTY MMPENGINE_CORE_NAME)
	get_property(frontend_name_tmp GLOBAL PROPERTY MMPENGINE_FRONTEND_NAME)

	target_include_directories(${appName} PRIVATE ${PROJECT_SOURCE_DIR}/libs/mmpengine/src)
	target_link_libraries(${appName} PRIVATE ${core_name_tmp} ${frontend_name_tmp})
	target_compile_features(${appName}  PRIVATE cxx_std_17)
	SET(RuntimeOutputDir ${CMAKE_SOURCE_DIR}/bin/${appName})
	set_property(TARGET ${appName} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${RuntimeOutputDir})
	
	IF (WIN32)
		get_property(win_macro_tmp GLOBAL PROPERTY MMPENGINE_WIN_MACRO)
		target_compile_definitions(${appName} PRIVATE ${win_macro_tmp}=1)
		set_target_properties(${appName} PROPERTIES WIN32_EXECUTABLE TRUE)
		SET(VSWorkingDir ${RuntimeOutputDir}/${CMAKE_CFG_INTDIR})
		set_property(TARGET ${appName} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${VSWorkingDir})
	elseif(APPLE)
		get_property(macosx_macro_tmp GLOBAL PROPERTY MMPENGINE_MACOSX_MACRO)
		target_compile_definitions(${appName} PRIVATE ${macosx_macro_tmp}=1)
	ENDIF()
	
endfunction()

addSample("Cube")