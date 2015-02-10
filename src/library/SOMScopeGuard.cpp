#include "SOMScopeGuard.hpp"


/*
This function constructs the scope guard and sets the function for it to call when it is deleted or goes out of scope.  The function argument must not throw exceptions.
@param inputFunction: The function to call when the object goes out of scope
*/
SOMScopeGuard::SOMScopeGuard(std::function<void()> inputFunction) : functionToCall(inputFunction)
{
   scopeGuardHasBeenDismissed = false;
}

/*
This function tells the scope guard that it should not call the function it was given when it goes out of scope.
*/
void SOMScopeGuard::dismiss()
{
   scopeGuardHasBeenDismissed = true;
}

/*
This destructor cleans up the object and calls the function that it was given on its creation.
*/
SOMScopeGuard::~SOMScopeGuard() noexcept
{
   if(!scopeGuardHasBeenDismissed)
   {
      functionToCall();
   }
}
