include_guard()

# Default: ON
option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors." ON)

include(cmake/compilers.cmake)

set(GCC_CLANG_BASE_WARNINGS
	-Wall
	-Wextra
	-Wpedantic
)

set(MSVC_BASE_WARNINGS
	/W4
)

function(set_target_warnings target)
	# Set warnings for a target
	#
	# Args:
	#	target: The target to set the warnings on.
	# Keyword args:
	#	UNKNOWN_MESSAGE: A message to print as a warning if the compiler is unknown.
	#	MSVC: The list of MSVC warning flags to set.
	#	CLANG: The list of Clang warning flags to set.
	#	GCC: The list of GCC warning flags to set.

	cmake_parse_arguments(
		PARSE_ARGV
			1
		WARNINGS
		""
		"TARGET;UNKNOWN_MESSAGE"
		"MSVC;CLANG;GCC"
	)

	if(USING_MSVC)
		set(warnings ${WARNINGS_MSVC})
	elseif(USING_CLANG)
		set(warnings ${WARNINGS_CLANG})
	elseif(USING_GCC)
		set(warnings ${WARNINGS_GCC})
	else()
		# Unknown compiler, warn user.
		message(WARNING "${UNKNOWN_MESSAGE}")
	endif()

	target_compile_options(
		${target}
		PRIVATE
			${warnings}
	)
endfunction()

function(set_default_warnings target)
	# Set a default warning set to a given target, in a compiler-independent way.

	set_target_warnings(
		${target}
		MSVC
			${MSVC_BASE_WARNINGS}
		CLANG
			${GCC_CLANG_BASE_WARNINGS}
		GCC
			${GCC_CLANG_BASE_WARNINGS}
		UNKNOWN_MESSAGE
			"Compiler is not known, cannot set warnings."
	)

	if(WARNINGS_AS_ERRORS)
		set_target_warnings(
			${target}
			MSVC
				"/WX"
			CLANG
				"-Werror"
			GCC
				"-Werror"
			UNKNOWN_MESSAGE
				"Compiler is not known, cannot set warnings as errors."
		)
	endif()
endfunction()
