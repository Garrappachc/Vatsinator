find_package (Qt5Core REQUIRED)
include_directories (${Qt5Core_INCLUDE_DIRS})

find_package (Qt5Network REQUIRED)
include_directories (${Qt5Network_INCLUDE_DIRS})

set (core_SOURCES
    airline.cpp
    airlinelistreader.cpp
    airport.cpp
    airportlistreader.cpp
    airportobject.cpp
    aliaslistreader.cpp
    atc.cpp
    atcbooking.cpp
    client.cpp
    clientdata.cpp
    clientlist.cpp
    filedownloader.cpp
    filereader.cpp
    fir.cpp
    firlistreader.cpp
    firobject.cpp
    geo.cpp
    lonlat.cpp
    metar.cpp
    metarmanager.cpp
    option.cpp
    pilot.cpp
    pluginfinder.cpp
    resourcefile.cpp
    servertracker.cpp
    tmalistreader.cpp
    uirlistreader.cpp
    vatsimdatadocument.cpp
    vatsimdatareader.cpp
    vatsimstatusdocument.cpp
    vatsimstatusreader.cpp
)

set (core_HEADERS
    airline.h
    airlinelistreader.h
    airport.h
    airportlistreader.h
    airportobject.h
    aliaslistreader.h
    atc.h
    atcbooking.h
    client.h
    clientdata.h
    clientlist.h
    filedownloader.h
    filereader.h
    fir.h
    firlistreader.h
    firobject.h
    geo.h
    lonlat.h
    metar.h
    metarmanager.h
    option.h
    pilot.h
    pluginfinder.h
    resourcefile.h
    servertracker.h
    tmalistreader.h
    uirlistreader.h
    vatsimdatadocument.h
    vatsimdatareader.h
    vatsimstatusdocument.h
    vatsimstatusreader.h
    
    core.h
    coreexport.h
)

set (core_PRIVATE_HEADERS

)

add_library (vtrcore SHARED ${core_SOURCES} ${core_HEADERS} ${core_PRIVATE_HEADERS})
target_link_libraries (vtrcore Qt5::Core Qt5::Network)
set_target_properties (vtrcore PROPERTIES CXX_VISIBILITY_PRESET hidden)
install (TARGETS vtrcore
    EXPORT vtrcore
    RUNTIME DESTINATION bin CONFIGURATIONS Debug Release
    LIBRARY DESTINATION lib/vatsinator CONFIGURATIONS Debug
    ARCHIVE DESTINATION lib CONFIGURATIONS Debug
)

add_subdirectory (resources)