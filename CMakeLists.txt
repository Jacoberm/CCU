# CMake input file to build ProdExplUnits

#include ose library
INCLUDE_DIRECTORIES( ${ECOS_INCLUDE}/include )
INCLUDE_DIRECTORIES( ${BASICLIBINCLUDE}/LibUtilities/include )
INCLUDE_DIRECTORIES( ${BASICLIBINCLUDE}/LibModules/include )
INCLUDE_DIRECTORIES( ${BASICLIBINCLUDE}/LibData/include )

INCLUDE_DIRECTORIES( ${PROD_EXPL_SOURCE_DIR}/ProdExplLib/include )

# Make sure the linker can find the libraries once they are built.
LINK_DIRECTORIES(${CMAKE_INSTALL_PREFIX}/lib)
#LINK_DIRECTORIES(${BASICLIBLIBRARY}/LibUtilities)
#LINK_DIRECTORIES(${BASICLIBLIBRARY}/LibModules)
LINK_DIRECTORIES(${ECOS_LIBRARY}/include)
LINK_DIRECTORIES(${ECOS_LIBRARY}/lib)

#AUX_SOURCE_DIRECTORY( . PROD_EXPL_UNITS_SRCS )
SET(PROD_EXPL_UNITS_SRCS
  shl5-683/ccu_emerg.c
  shl5-683/ccu_station.c
  shl5-683/ccu_tms.c
  shl5-683/manual_ann.c
  shl5-683/media_and_monitor.c 
  shl5-683/ccu_led.c  
  shl5-683/simulate_station_code.c
  shl5-683/alarm_list_fun.c
  shl5-683/thread_comm.c
  shl5-683/thread_config.c
  shl5-683/ccu_log_info.c
  shl5-683/file_operate.c
  shl5-683/debug_network.c
	
  shl5-683/buffer_read_write_fun.c
  shl5-683/sound_driver.c
  shl5-683/log_record.c	
  shl5-683/ccu_tcms_test.c	
  shl5-683/function_for_network.c
	
  shl5-683/network/zhw_buffer_init.c
  shl5-683/network/zhw_dev_config_info.c
  shl5-683/network/zhw_dev_selftest_module.c
  shl5-683/network/zhw_middle_forward.c
  shl5-683/network/zhw_mount_romfs.c
  shl5-683/network/zhw_network_main.c
  shl5-683/network/zhw_read_from_db_test.c
  shl5-683/network/zhw_set_into_mul.c
  shl5-683/network/zhw_socket_init.c
  shl5-683/network/zhw_sockfd_server_init.c
  shl5-683/network/zhw_use_ioctl_to_set_net.c
  shl5-683/network/add_ip_to_mul.c
  shl5-683/network/udp_recv_fun.c
  shl5-683/network/udp_send_fun.c
  shl5-683/network/udp_recv_fun.c
  shl5-683/network/zhw_apply_common_func.c
  shl5-683/network/zhw_shell_function.c	
	
  shl5-683/sqlite3.c
  shl5-683/sqlite_api_fun.c
  shl5-683/sqlite_middle_layer.c
  shl5-683/sqlite_select.c
  shl5-683/sqlite_convert_fill_fun.c
  shl5-683/tftp_update_protect_confige.c
  shl5-683/tftp_client_apply.c	
  shl5-683/tftp_client_fun.c
  shl5-683/tftp_network_base.c
  shl5-683/tftp_server_apply.c
  shl5-683/tftp_server_fun.c 	
	
  shl5-683/ccu_main.c
  shl5-683/ccu_function.c	
  shl5-683/ccu_gd_sync.c
  shl5-683/ccu_ann_fun.c
  shl5-683/ccu_state_machine.c
  shl5-683/ccu_device_volume.c
) 

# Strip release build
IF( "${CMAKE_BUILD_TYPE}" STREQUAL "Release" )
  SET( CMAKE_EXE_LINKER_FLAGS  "-Wl,-strip-all")
ENDIF( "${CMAKE_BUILD_TYPE}" STREQUAL "Release" )
#SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)

ADD_EXECUTABLE(ProdTest ${PROD_EXPL_UNITS_SRCS})

# Link the executable to the libraries.
TARGET_LINK_LIBRARIES(ProdTest
#  ${CMAKE_INSTALL_PREFIX}/lib/LibModules.a
)
TARGET_LINK_LIBRARIES(ProdTest  target )



INSTALL( TARGETS ProdTest  RUNTIME DESTINATION ./ )

INSTALL( CODE "
	EXECUTE_PROCESS(COMMAND rm  ProdTest_${BUILD}.srec ProdTest_${BUILD}.bin  ProdTest_${BUILD}.img ProdTest_${BUILD}.elf WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/)	
#	EXECUTE_PROCESS(COMMAND rm ProdTest_${BUILD}.srec ProdTest_${BUILD}.bin  ProdTest_${BUILD}.img ProdTest_${BUILD}.elf   WORKING_DIRECTORY  /home/benty/workspace/Products/${BUILD}/)	

	
	EXECUTE_PROCESS(COMMAND ${CROSSTOOL}/${TOOLS_PREFIX}/bin/${TOOLS_PREFIX}-objcopy -O  binary ProdTest.elf ProdTest_${BUILD}.bin WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/)
	EXECUTE_PROCESS(COMMAND ${CROSSTOOL}/${TOOLS_PREFIX}/bin/${TOOLS_PREFIX}-objcopy -O srec ProdTest.elf ProdTest_${BUILD}.srec WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/)
	EXECUTE_PROCESS(COMMAND cp	ProdTest.elf ./ProdTest_${BUILD}.elf WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/)
#	EXECUTE_PROCESS(COMMAND	dd if=/dev/zero of=ProdTest_${BUILD}.img bs=512 count=2880 WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/)
#	EXECUTE_PROCESS(COMMAND dd if=ProdTest_${BUILD}.bin of=ProdTest_${BUILD}.img bs=512 conv=notrunc WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/)
	EXECUTE_PROCESS(COMMAND cp  ProdTest_${BUILD}.srec ProdTest_${BUILD}.bin  ProdTest_${BUILD}.img ProdTest_${BUILD}.elf /home/benty/workspace/Products/${BUILD}/   WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/)
#	EXECUTE_PROCESS(COMMAND rm ProdTest.elf WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/)	
")

