set(BOOST_ROOT "T:/orther/graphics/boost_1_84_0")
set(Boost_USE_STATIC_LIBS ON )
set(Boost_USE_MULTITHREADED ON )
set(Boost_ARCHITECTURE "-x64" )


IF (Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
    message("Find package boost")
    ADD_DEFINITIONS( "-DHAS_BOOST" )
ENDIF()