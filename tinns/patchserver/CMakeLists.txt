ADD_EXECUTABLE          (PatchServer Client.cxx Includes.cxx PatchServer.cxx Main.cxx Server.cxx)
TARGET_LINK_LIBRARIES   (PatchServer Common ${PCRE_LIBRARY} ${RT_LIBRARY} ${ZLIB_LIBRARY})
