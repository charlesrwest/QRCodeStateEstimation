#ifndef SOMSCOPEGUARDHPP
#define SOMSCOPEGUARDHPP

#include <functional>
#include<cstdio>


/*
This class is inspired by the scope guard concept created by Andrei Alexandrescu and Petru Marginean.  It allows simple use of RAII (resource allocation is initialization) by letting the programmer create a scope guard containing a cleanup function right after the allocate a resource.  The function will then be called by the scope guard destructor as the scope guard object goes out of scope (any exit from the scope that the scope guard was declared in).  This ensures that allocated resources are freed when the scope returns no matter what causes the scope to return (such as a thrown exception).  It is particularly handy if you use lambdas to write quick freeing code.

If some change of state occurs so that the function should not be called, just call dismiss on the scope guard object to cancel execution.

As the given function is called from a destructor, it is not allowed to throw any exceptions. 
*/
class SOMScopeGuard
{
public:

/*
This function constructs the scope guard and sets the function for it to call when it is deleted or goes out of scope.  The function argument must not throw exceptions.
@param inputFunction: The function to call when the object goes out of scope
*/
SOMScopeGuard(std::function<void()> inputFunction);

/*
This function tells the scope guard that it should not call the function it was given when it goes out of scope.
*/
void dismiss();

/*
This destructor cleans up the object and calls the function that it was given on its creation.
*/
~SOMScopeGuard() noexcept;


private:
SOMScopeGuard() = delete; //Disable creation of the object without a function argument
SOMScopeGuard(const SOMScopeGuard &inputSOMScopeGuard) = delete; //Disable copying of the object

bool scopeGuardHasBeenDismissed;
std::function<void()> functionToCall;
};












#endif
