cmake_minimum_required(VERSION 3.30)

find_package(DD4hep 1.18 REQUIRED COMPONENTS DDCore DDG4)

dd4hep_add_plugin(ddDIRCactionStep
    SOURCES *.cxx
    USES    DD4hep::DDG4 
    )
install(TARGETS ddDIRCactionStep LIBRARY DESTINATION lib)
