#pragma once

// === CGI ===
#include "cgi/AllocationError.hpp"
#include "cgi/CloseFailedChild.hpp"
#include "cgi/CloseFailedParent.hpp"
#include "cgi/CreateFdRequestFailed.hpp"
#include "cgi/CreateFdResponseFailed.hpp"
#include "cgi/DupFailedChild.hpp"
#include "cgi/DupFailedParent.hpp"
#include "cgi/ExecFailed.hpp"
#include "cgi/ForkFailed.hpp"
#include "cgi/InvalidEnvValue.hpp"
#include "cgi/NoSuchFile.hpp"
#include "cgi/PipeFailed.hpp"
#include "cgi/WaitPidFailed.hpp"
#include "cgi/ChildTimeout.hpp"
#include "cgi/WriteIntoFdFailed.hpp"

// === HTTP Exceptions ===
#include "http/InvalidHeaderFormat.hpp"
#include "http/InvalidMethod.hpp"
#include "http/InvalidRequestLine.hpp"
#include "http/DuplicateHeader.hpp"
#include "http/FileAccessDenied.hpp"
#include "http/FileNotFound.hpp"
#include "http/FileTraversalExploit.hpp"
#include "http/UriTooLong.hpp"
#include "http/InvalidPath.hpp"
#include "http/ContentLengthTooLong.hpp"
#include "http/ConversionFailure.hpp"
#include "http/ContentOfCurrentPageHasBeenDeleted.hpp"
#include "http/Forbidden.hpp"
#include "http/ErrorDeletingFile.hpp"
#include "http/Redirecting.hpp"
#include "http/InvalidUploadDir.hpp"
#include "http/InvalidBoundary.hpp"



// CONFIG IMPLEMENTATION - START
#include "http/InvalidCfgRoot.hpp" 
#include "http/InvalidCfgLocation.hpp"
// CONFIG IMPLEMENTATION - END

// === Encoding Exceptions ===
#include "encoding/InvalidEncodedUri.hpp"

// === IO Exceptions ===

//  == Socket Exceptions ==
#include "io/socket/AddrUnsupported.hpp"
#include "io/socket/FileLimitReached.hpp"
#include "io/socket/InvalidFlags.hpp"
#include "io/socket/InvalidProtocol.hpp"
#include "io/socket/OutOfMemory.hpp"
#include "io/socket/PermissionDenied.hpp"
#include "io/socket/ProtocolUnsupported.hpp"
#include "io/socket/UnknownError.hpp"
#include "io/socket/InvalidAddress.hpp"
#include "io/socket/AddrInUse.hpp"
#include "io/socket/InvalidDescriptor.hpp"
#include "io/socket/AddrNotAvailable.hpp"
#include "io/socket/ConnectionAborted.hpp"
#include "io/socket/Interrupted.hpp"
#include "io/socket/NotListening.hpp"
#include "io/socket/ProtocolError.hpp"
#include "io/socket/InvalidPort.hpp"
#include "io/socket/PipeClosed.hpp"


// === Config Exceptions ===
#include "config/InvalidMethods.hpp"
#include "config/EmptyFile.hpp"
#include "config/InvalidFileName.hpp"
#include "config/ConfigError.hpp"

