cmake_minimum_required(VERSION 3.10)
project(mmpengine_basic_samples VERSION 1.0.0 LANGUAGES C CXX)
include(FindVulkan)

IF (WIN32)
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
endif()

IF(${Vulkan_FOUND})
	find_program(GLSLANG_VALIDATOR glslangValidator HINTS ENV VULKAN_SDK)

	if (NOT GLSLANG_VALIDATOR)
		message(FATAL_ERROR "glslangValidator not found. Please install Vulkan SDK.")
	endif()
	
endif()

add_subdirectory(libs/mmpengine)

function(addSample appName)

	add_executable(${appName}
		src/samples/${appName}/Main.cpp
	)
	
	mmpengine_add_source_files_to_proj(${appName} src/samples/${appName})

	target_include_directories(${appName} PRIVATE ${PROJECT_SOURCE_DIR}/libs/mmpengine/src ${PROJECT_SOURCE_DIR}/src/samples)
	target_compile_features(${appName}  PRIVATE cxx_std_17)
	SET(RuntimeOutputDir ${CMAKE_SOURCE_DIR}/bin/${appName})
	set_property(TARGET ${appName} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${RuntimeOutputDir})

	mmpengine_add_dependencies_and_macro(${appName})


	IF (WIN32)
		set_target_properties(${appName} PROPERTIES WIN32_EXECUTABLE TRUE)
		
		if (CMAKE_GENERATOR MATCHES "Visual Studio")
			SET(VSWorkingDir ${RuntimeOutputDir}/${CMAKE_CFG_INTDIR})
			set_property(TARGET ${appName} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${VSWorkingDir})
		else()
			#TODO: support
		endif()
	ELSEIF(APPLE)
		if(CMAKE_GENERATOR STREQUAL "Xcode")
    			set_target_properties(${appName} PROPERTIES
        		XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO"
        		XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED "NO"
        		XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "")
		endif()
	
	ENDIF()

	#shdder configs
	file (GLOB shaderConfigurationFiles src/samples/${appName}/shaders/*.json)
	
	foreach( shader_cfg ${shaderConfigurationFiles})
		target_sources(${appName} PRIVATE ${shader_cfg})
		
		if (CMAKE_GENERATOR MATCHES "Visual Studio")
			set_source_files_properties( ${shader_cfg} PROPERTIES VS_COPY_TO_OUT_DIR "Always")
		else()
			#TODO: support
		endif()
		
	endforeach( shader_cfg ${shaderConfigurationFiles} )
	
	
	IF (WIN32)
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
	ENDIF()
	
	IF (NOT APPLE)
		IF(${Vulkan_FOUND})
			set(shader_files_folder src/samples/${appName}/shaders)
			file (GLOB shaderFiles 
				${shader_files_folder}/*.vert
				${shader_files_folder}/*.frag
				${shader_files_folder}/*.comp
			)
			foreach( shader_file ${shaderFiles})
				target_sources(${appName} PRIVATE ${shader_file})
				
				if (CMAKE_GENERATOR MATCHES "Visual Studio")
					get_filename_component(filename ${shader_file} NAME)
					set(output_spirv "${VSWorkingDir}/${filename}.spv")
					add_custom_command(
						TARGET ${appName} POST_BUILD
						COMMAND glslc -Ishaders/includes ${shader_file} -o ${output_spirv}
						COMMENT "Compiling GLSL shader ${shader_file} to ${output_spirv};"
					)
					
				else()
					#TODO: support
				endif()
						
			endforeach( shader_file ${shaderFiles} )
		ENDIF()
	ENDIF()
	
endfunction()

addSample("Empty")
addSample("Boxes")
addSample("Buffers")
addSample("Compute")