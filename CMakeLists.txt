cmake_minimum_required(VERSION 2.6)

project(time-tracker)

# Adds header files to global HEADER_LIST property
get_property(VAR_HEADER_LIST GLOBAL PROPERTY HEADER_LIST)
set_property(GLOBAL PROPERTY HEADER_LIST
	${VAR_HEADER_LIST}
	time-tracker/TimeTracker.h)

install(FILES TimeTracker.h DESTINATION include/sadehghan)
