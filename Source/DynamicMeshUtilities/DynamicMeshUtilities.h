#pragma once

// these "errors" are not functionally wrong in most situations and thus should only be a notification instead of preventing compilation
#pragma warning(once: 4065) // switch statement contains 'default' but no 'case' labels
#pragma warning(once: 4390) // empty control statement found

class FDynamicMeshUtilitiesModule : public IModuleInterface {};
