cmake_minimum_required(VERSION 3.7)
project(mmpengine_basic_samples VERSION 1.0.0 LANGUAGES C CXX)

IF (WIN32)
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
endif()


add_subdirectory(libs/mmpengine)

function(addSample appName)

	add_executable(${appName}
		src/samples/${appName}/Main.cpp
	)
	
	mmpengine_add_source_files_to_proj(${appName} src/samples/${appName})

	get_property(core_name_tmp GLOBAL PROPERTY MMPENGINE_CORE_NAME)
	get_property(frontend_name_tmp GLOBAL PROPERTY MMPENGINE_FRONTEND_NAME)
	get_property(feature_name_tmp GLOBAL PROPERTY MMPENGINE_FEATURE_NAME)

	target_include_directories(${appName} PRIVATE ${PROJECT_SOURCE_DIR}/libs/mmpengine/src ${PROJECT_SOURCE_DIR}/src/samples)
	target_link_libraries(${appName} PRIVATE ${core_name_tmp} ${frontend_name_tmp} ${feature_name_tmp})
	target_compile_features(${appName}  PRIVATE cxx_std_17)
	SET(RuntimeOutputDir ${CMAKE_SOURCE_DIR}/bin/${appName})
	set_property(TARGET ${appName} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${RuntimeOutputDir})

	IF (NOT APPLE)
		include(FindVulkan)
		IF(${Vulkan_FOUND})
			get_property(vulkan_macro_tmp GLOBAL PROPERTY MMPENGINE_VULKAN_MACRO)
			target_compile_definitions(${appName} PRIVATE ${vulkan_macro_tmp}=1)
		ENDIF()
	ENDIF()

	IF (WIN32)
		get_property(win_macro_tmp GLOBAL PROPERTY MMPENGINE_WIN_MACRO)
		get_property(dx12_macro_tmp GLOBAL PROPERTY MMPENGINE_DX12_MACRO)
		target_compile_definitions(${appName} PRIVATE ${win_macro_tmp}=1)
		target_compile_definitions(${appName} PRIVATE ${dx12_macro_tmp}=1)
		
		set_target_properties(${appName} PROPERTIES WIN32_EXECUTABLE TRUE)
		
		if (CMAKE_GENERATOR MATCHES "Visual Studio")
			SET(VSWorkingDir ${RuntimeOutputDir}/${CMAKE_CFG_INTDIR})
			set_property(TARGET ${appName} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${VSWorkingDir})
		else()
			#TODO: support
		endif()
		
	elseif(APPLE)
		get_property(macosx_macro_tmp GLOBAL PROPERTY MMPENGINE_MACOSX_MACRO)
		target_compile_definitions(${appName} PRIVATE ${macosx_macro_tmp}=1)
	ENDIF()

	#shdders
	file (GLOB shaderConfigurationFiles src/samples/${appName}/shaders/*.json)
	
	foreach( shader_cfg ${shaderConfigurationFiles})
		target_sources(${appName} PRIVATE ${shader_cfg})
		
		if (CMAKE_GENERATOR MATCHES "Visual Studio")
			set_source_files_properties( ${shader_cfg} PROPERTIES VS_COPY_TO_OUT_DIR "Always")
		else()
			#TODO: support
		endif()
		
	endforeach( shader_cfg ${shaderConfigurationFiles} )
	
	#hlsl shaders
	list(APPEND hlsl_shader_types Vertex Pixel Geometry Hull Domain Compute Amplification Mesh Library)
	file (GLOB hlslShaders src/samples/${appName}/shaders/*.hlsl)
	foreach( hlsl_shader ${hlslShaders})
	
		target_sources(${appName} PRIVATE ${hlsl_shader})
		
		get_filename_component(shaderNameWE ${hlsl_shader} NAME_WE)
		
		SET(shaderType "unknown")
		
		foreach(Type IN LISTS hlsl_shader_types)
			if(${shaderNameWE} MATCHES "^${Type}_")
				SET(shaderType ${Type})
			endif()
		endforeach()
		
		if (CMAKE_GENERATOR MATCHES "Visual Studio")
			set_source_files_properties( ${hlsl_shader} PROPERTIES VS_SHADER_ENTRYPOINT "EntryPoint")
			set_source_files_properties( ${hlsl_shader} PROPERTIES VS_SHADER_TYPE ${shaderType})
			set_source_files_properties( ${hlsl_shader} PROPERTIES VS_SHADER_MODEL "6.5")
		else()
			#TODO: support
		endif()

	endforeach( hlsl_shader ${hlslShaders} )
	
endfunction()

addSample("Empty")
addSample("Boxes")
addSample("Buffers")
addSample("Compute")