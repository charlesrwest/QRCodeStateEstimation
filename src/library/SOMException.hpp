#ifndef SOMEXCEPTIONHPP
#define SOMEXCEPTIONHPP


#ifndef SOM_TRY
#define SOM_TRY try {
#endif

#ifndef SOM_CATCH
#define SOM_CATCH(inputHigherLevelErrorMessage) } catch(SOMException &inputSOMException) { throw SOMException(inputHigherLevelErrorMessage, inputSOMException, __FILE__, __LINE__); } catch(const std::exception &inputException) { throw SOMException(inputHigherLevelErrorMessage, inputException, __FILE__, __LINE__); }

#endif

#ifndef SOM_CATCH2
#define SOM_CATCH2(inputHigherLevelErrorMessage, inputExceptionClass) } catch(SOMException &inputSOMException) { throw SOMException(inputHigherLevelErrorMessage, inputExceptionClass, inputSOMException, __FILE__, __LINE__); } catch(const std::exception &inputException) { throw SOMException(inputHigherLevelErrorMessage, inputExceptionClass, inputException, __FILE__, __LINE__); }
#endif

#include<string>

enum exceptionClass
{
ZMQ_ERROR,
SQLITE3_ERROR,
ZBAR_ERROR,
FILE_SYSTEM_ERROR,
AN_ASSUMPTION_WAS_VIOLATED_ERROR,
SINGLETON_ALREADY_EXISTS,
SINGLETON_CREATION_FAILED,
FORK_ERROR,
SYSTEM_ERROR,
INVALID_FUNCTION_INPUT, //Value given to a function which renders is output/operation invalid
INCORRECT_SERVER_RESPONSE,
SERVER_REQUEST_FAILED,
UNKNOWN
};

/*
This class is used to throw an informative exception.
*/
class SOMException : public std::exception
{
public:
/*
This function initializes the exception object with the required information.
@param inputErrorMessage: A message specific to this error instance
@param inputExceptionClass: What class of error this is (so different types of processing can be done)
@param inputSourceFileName: The name of the source file this exception originated in (hopefully provided by the __FILE__ macro)
@param inputSourceLineNumber: The line that the exception originated (hopefully provided by the __LINE__ macro)
*/
SOMException(const std::string &inputErrorMessage, exceptionClass inputExceptionClass, const char *inputSourceFileName, int inputSourceLineNumber);

/*
This function initializes the exception object with an error message that is the concatenation of the given error message and the given exception.
@param inputErrorMessage: A message specific to this error instance
@param inputExceptionClass: What class of error this is (so different types of processing can be done)
@param inputException: The exception received from a lower level that we are rethrowing as a SOMException with more detail
@param inputSourceFileName: The name of the source file this exception originated in (hopefully provided by the __FILE__ macro)
@param inputSourceLineNumber: The line that the exception originated (hopefully provided by the __LINE__ macro)
*/
SOMException(const std::string &inputErrorMessage, exceptionClass inputExceptionClass, const std::exception &inputException, const char *inputSourceFileName, int inputSourceLineNumber);

/*
This function initializes the exception object with an error message that is the concatenation of the given error message and the given exception with an unknown exception class.
@param inputErrorMessage: A message specific to this error instance
@param inputException: The exception received from a lower level that we are rethrowing as a SOMException with more detail
@param inputSourceFileName: The name of the source file this exception originated in (hopefully provided by the __FILE__ macro)
@param inputSourceLineNumber: The line that the exception originated (hopefully provided by the __LINE__ macro)
*/
SOMException(const std::string &inputErrorMessage, const std::exception &inputException, const char *inputSourceFileName, int inputSourceLineNumber);

/*
This function initializes the exception object with an error message that is the concatenation of the given error message and the given SOMException with the given SOMException's error class.
@param inputErrorMessage: A message specific to this error instance
@param inputExceptionClass: What class of error this is (so different types of processing can be done)
@param inputSOMException: The SOMException received from a lower level that we are rethrowing as a SOMException with more detail
@param inputSourceFileName: The name of the source file this exception originated in (hopefully provided by the __FILE__ macro)
@param inputSourceLineNumber: The line that the exception originated (hopefully provided by the __LINE__ macro)
*/
SOMException(const std::string &inputErrorMessage, exceptionClass inputExceptionClass, const SOMException &inputSOMException, const char *inputSourceFileName, int inputSourceLineNumber);

/*
This function initializes the exception object with an error message that is the concatenation of the given error message and the given SOMException with the given SOMException's error class.
@param inputErrorMessage: A message specific to this error instance
@param inputSOMException: The SOMException received from a lower level that we are rethrowing as a SOMException with more detail
@param inputSourceFileName: The name of the source file this exception originated in (hopefully provided by the __FILE__ macro)
@param inputSourceLineNumber: The line that the exception originated (hopefully provided by the __LINE__ macro)
*/
SOMException(const std::string &inputErrorMessage, const SOMException &inputSOMException, const char *inputSourceFileName, int inputSourceLineNumber);

/*
This function returns a string that is a summary of the error that caused the exception
@return: The string summary of the error
*/
std::string toString() const;

/*
This function overrides the virtual function that was defined in std::exception and returns the result of toString() as a const string
@return: The string summary of the error
*/
virtual const char *what() const throw();

std::string errorMessage;
exceptionClass exceptionType;
std::string sourceFileName;
std::string sourceLineNumber;
};

/*
This function converts the exceptionClass enum into a string
@param inputExceptionType: The type of exception
@return: The string equivalent
*/
std::string exceptionClassToString(exceptionClass inputExceptionType);

/*
It turns out that this functionality is possible but fairly complex, so it is being left for a later release.  A useful resource on how to implement it can be found at these places: 

http://akrzemi1.wordpress.com/2011/05/11/parsing-strings-at-compile-time-part-i/

https://groups.google.com/forum/?hl=en#!topic/comp.lang.c++.moderated/2b3dQQXxRB4

The FILE macro normally returns the entire path for a source file.  This constexpr function trims that at compile time to the base name of the source file.
*/
//constexpr const char *trimToBaseName(const char *inputFullFilePath);




#endif
