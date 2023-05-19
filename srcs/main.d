srcs/main.o: srcs/main.cpp includes/Response.hpp includes/Request.hpp \
  includes/config/configFile.hpp includes/colors.hpp \
  includes/config/location.hpp includes/exceptions.hpp \
  includes/exceptions/cgi/AllocationError.hpp \
  includes/exceptions/cgi/CloseFailedChild.hpp \
  includes/exceptions/cgi/CloseFailedParent.hpp \
  includes/exceptions/cgi/CreateFdRequestFailed.hpp \
  includes/exceptions/cgi/CreateFdResponseFailed.hpp \
  includes/exceptions/cgi/DupFailedChild.hpp \
  includes/exceptions/cgi/DupFailedParent.hpp \
  includes/exceptions/cgi/ExecFailed.hpp \
  includes/exceptions/cgi/ForkFailed.hpp \
  includes/exceptions/cgi/InvalidEnvValue.hpp \
  includes/exceptions/cgi/NoSuchFile.hpp \
  includes/exceptions/cgi/PipeFailed.hpp \
  includes/exceptions/cgi/WaitPidFailed.hpp \
  includes/exceptions/cgi/ChildTimeout.hpp \
  includes/exceptions/cgi/WriteIntoFdFailed.hpp \
  includes/exceptions/http/InvalidHeaderFormat.hpp \
  includes/exceptions/http/InvalidMethod.hpp \
  includes/exceptions/http/InvalidRequestLine.hpp \
  includes/exceptions/http/DuplicateHeader.hpp \
  includes/exceptions/http/FileAccessDenied.hpp \
  includes/exceptions/http/FileNotFound.hpp \
  includes/exceptions/http/FileTraversalExploit.hpp \
  includes/exceptions/http/UriTooLong.hpp \
  includes/exceptions/http/InvalidPath.hpp \
  includes/exceptions/http/ContentLengthTooLong.hpp \
  includes/exceptions/http/ConversionFailure.hpp \
  includes/exceptions/http/ContentOfCurrentPageHasBeenDeleted.hpp \
  includes/exceptions/http/Forbidden.hpp \
  includes/exceptions/http/ErrorDeletingFile.hpp \
  includes/exceptions/http/Redirecting.hpp \
  includes/exceptions/http/InvalidUploadDir.hpp \
  includes/exceptions/http/InvalidBoundary.hpp \
  includes/exceptions/http/InvalidCfgRoot.hpp \
  includes/exceptions/http/InvalidCfgLocation.hpp \
  includes/exceptions/encoding/InvalidEncodedUri.hpp \
  includes/exceptions/io/socket/AddrUnsupported.hpp \
  includes/exceptions/io/socket/FileLimitReached.hpp \
  includes/exceptions/io/socket/InvalidFlags.hpp \
  includes/exceptions/io/socket/InvalidProtocol.hpp \
  includes/exceptions/io/socket/OutOfMemory.hpp \
  includes/exceptions/io/socket/PermissionDenied.hpp \
  includes/exceptions/io/socket/ProtocolUnsupported.hpp \
  includes/exceptions/io/socket/UnknownError.hpp \
  includes/exceptions/io/socket/InvalidAddress.hpp \
  includes/exceptions/io/socket/AddrInUse.hpp \
  includes/exceptions/io/socket/InvalidDescriptor.hpp \
  includes/exceptions/io/socket/AddrNotAvailable.hpp \
  includes/exceptions/io/socket/ConnectionAborted.hpp \
  includes/exceptions/io/socket/Interrupted.hpp \
  includes/exceptions/io/socket/NotListening.hpp \
  includes/exceptions/io/socket/ProtocolError.hpp \
  includes/exceptions/io/socket/InvalidPort.hpp \
  includes/exceptions/io/socket/PipeClosed.hpp \
  includes/exceptions/config/InvalidMethods.hpp \
  includes/exceptions/config/EmptyFile.hpp \
  includes/exceptions/config/InvalidFileName.hpp \
  includes/exceptions/config/ConfigError.hpp includes/config/server.hpp \
  includes/Socket.hpp includes/Cgi.hpp includes/logger.hpp \
  includes/MetaVariable.hpp includes/utils.hpp includes/constant.hpp \
  includes/codes.hpp
