ADD_EXECUTABLE          (InfoServer Accounts.cxx Client.cxx Includes.cxx InfoServer.cxx Main.cxx Server.cxx Sql.cxx)
TARGET_LINK_LIBRARIES   (InfoServer Common ${MYSQL_LIBRARY} ${PCRE_LIBRARY})
