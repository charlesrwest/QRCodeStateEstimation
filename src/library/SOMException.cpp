#include "SOMException.hpp"

/*
This function initializes the exception object with the required information.
@param inputErrorMessage: A message specific to this error instance
@param inputExceptionClass: What class of error this is (so different types of processing can be done)
@param inputSourceFileName: The name of the source file this exception originated in (hopefully provided by the __FILE__ macro)
@param inputSourceLineNumber: The line that the exception originated (hopefully provided by the __LINE__ macro)
*/
SOMException::SOMException(const std::string &inputErrorMessage, exceptionClass inputExceptionClass, const char *inputSourceFileName, int inputSourceLineNumber) : errorMessage(inputErrorMessage)
{

exceptionType = inputExceptionClass;

if(inputSourceFileName != NULL)
{
sourceFileName = std::string(inputSourceFileName);
}

sourceLineNumber = std::to_string(inputSourceLineNumber);

}

/*
This function initializes the exception object with an error message that is the concatenation of the given error message and the given exception.
@param inputErrorMessage: A message specific to this error instance
@param inputExceptionClass: What class of error this is (so different types of processing can be done)
@param inputException: The exception received from a lower level that we are rethrowing as a SOMException with more detail
@param inputSourceFileName: The name of the source file this exception originated in (hopefully provided by the __FILE__ macro)
@param inputSourceLineNumber: The line that the exception originated (hopefully provided by the __LINE__ macro)
*/
SOMException::SOMException(const std::string &inputErrorMessage, exceptionClass inputExceptionClass, const std::exception &inputException, const char *inputSourceFileName, int inputSourceLineNumber) : errorMessage(inputErrorMessage + inputException.what())
{
exceptionType = inputExceptionClass;

if(inputSourceFileName != NULL)
{
sourceFileName = std::string(inputSourceFileName);
}

sourceLineNumber = std::to_string(inputSourceLineNumber);
}

/*
This function initializes the exception object with an error message that is the concatenation of the given error message and the given exception with an unknown exception class.
@param inputErrorMessage: A message specific to this error instance
@param inputException: The exception received from a lower level that we are rethrowing as a SOMException with more detail
@param inputSourceFileName: The name of the source file this exception originated in (hopefully provided by the __FILE__ macro)
@param inputSourceLineNumber: The line that the exception originated (hopefully provided by the __LINE__ macro)
*/
SOMException::SOMException(const std::string &inputErrorMessage, const std::exception &inputException, const char *inputSourceFileName, int inputSourceLineNumber) : errorMessage(inputErrorMessage + inputException.what())
{
exceptionType = UNKNOWN;

if(inputSourceFileName != NULL)
{
sourceFileName = std::string(inputSourceFileName);
}

sourceLineNumber = std::to_string(inputSourceLineNumber);
}

/*
This function initializes the exception object with an error message that is the concatenation of the given error message and the given SOMException with the given SOMException's error class.
@param inputErrorMessage: A message specific to this error instance
@param inputSOMException: The SOMException received from a lower level that we are rethrowing as a SOMException with more detail
@param inputSourceFileName: The name of the source file this exception originated in (hopefully provided by the __FILE__ macro)
@param inputSourceLineNumber: The line that the exception originated (hopefully provided by the __LINE__ macro)
*/
SOMException::SOMException(const std::string &inputErrorMessage, const SOMException &inputSOMException, const char *inputSourceFileName, int inputSourceLineNumber) : errorMessage(inputErrorMessage + inputSOMException.toString())
{
exceptionType = inputSOMException.exceptionType;

if(inputSourceFileName != NULL)
{
sourceFileName = std::string(inputSourceFileName);
}

sourceLineNumber = std::to_string(inputSourceLineNumber);
}

/*
This function initializes the exception object with an error message that is the concatenation of the given error message and the given SOMException with the given SOMException's error class.
@param inputErrorMessage: A message specific to this error instance
@param inputExceptionClass: What class of error this is (so different types of processing can be done)
@param inputSOMException: The SOMException received from a lower level that we are rethrowing as a SOMException with more detail
@param inputSourceFileName: The name of the source file this exception originated in (hopefully provided by the __FILE__ macro)
@param inputSourceLineNumber: The line that the exception originated (hopefully provided by the __LINE__ macro)
*/
SOMException::SOMException(const std::string &inputErrorMessage, exceptionClass inputExceptionClass, const SOMException &inputSOMException, const char *inputSourceFileName, int inputSourceLineNumber) : errorMessage(inputErrorMessage + inputSOMException.toString())
{
exceptionType = inputExceptionClass;

if(inputSourceFileName != NULL)
{
sourceFileName = std::string(inputSourceFileName);
}

sourceLineNumber = std::to_string(inputSourceLineNumber);
}



/*
This function returns a string that is a summary of the error that caused the exception
@return: The string summary of the error
*/
std::string SOMException::toString() const
{
return "Error of type " + exceptionClassToString(exceptionType) + " occurred in file " + sourceFileName + " at line " + sourceLineNumber + ": " + errorMessage;
}

/*
This function overrides the virtual function that was defined in std::exception and returns the result of toString() as a const string
@return: The string summary of the error
*/
const char *SOMException::what() const throw()
{
return toString().c_str();
}


/*
This function converts the exceptionClass enum into a string
@param inputExceptionType: The type of exception
@return: The string equivalent
*/
std::string exceptionClassToString(exceptionClass inputExceptionType)
{
switch(inputExceptionType)
{
case ZMQ_ERROR:
return std::string("ZMQ_ERROR");
break;

case SQLITE3_ERROR:
return std::string("ZMQ_ERROR");
break;

case FILE_SYSTEM_ERROR:
return std::string("FILE_SYSTEM_ERROR");
break;

case AN_ASSUMPTION_WAS_VIOLATED_ERROR:
return std::string("AN_ASSUMPTION_WAS_VIOLATED_ERROR");
break;

case SINGLETON_ALREADY_EXISTS:
return std::string("SINGLETON_ALREADY_EXISTS");
break;

case SINGLETON_CREATION_FAILED:
return std::string("SINGLETON_CREATION_FAILED");
break;

case FORK_ERROR:
return std::string("FORK_ERROR");
break;

case SYSTEM_ERROR:
return std::string("SYSTEM_ERROR");
break;

case INVALID_FUNCTION_INPUT:
return std::string("INVALID_FUNCTION_INPUT");
break;

case INCORRECT_SERVER_RESPONSE:
return std::string("INCORRECT_SERVER_RESPONSE");
break;

case SERVER_REQUEST_FAILED:
return std::string("SERVER_REQUEST_FAILED");
break;

case UNKNOWN:
return std::string("UNKNOWN");
break;

default:
return "";
}
}



