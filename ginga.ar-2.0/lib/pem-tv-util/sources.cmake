

SET( ${PROJECT_NAME}_HEADER_FILES
	include/pem_log_config.h
	include/pem_log.h
	include/pem_moa_common.h
	include/pem_moa_msg.h

	include/pem_port_osa.h

	include/ginga_n_port_data.h
	include/ginga_n2tv_api.h
)

SET( ${PROJECT_NAME}_SOURCE_FILES
	${${PROJECT_NAME}_HEADER_FILES}
	src/pem_log.c
	src/pem_log_config.c
	src/pem_port_osa.c
	src/pem_moa_msg.c
)
