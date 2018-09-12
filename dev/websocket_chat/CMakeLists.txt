set(SAMPLE websocket_chat_app)
set(SAMPLE_SRCFILES main.cpp)

add_executable(${SAMPLE} ${SAMPLE_SRCFILES})

TARGET_LINK_LIBRARIES(${SAMPLE} PRIVATE restinio::restinio)

link_threads_if_necessary(${SAMPLE})

IF (WIN32)
	TARGET_LINK_LIBRARIES(${SAMPLE} PRIVATE wsock32 ws2_32)
ENDIF ()

install(TARGETS ${SAMPLE} DESTINATION bin)
