#include "porous_modelApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"

InputParameters
porous_modelApp::validParams()
{
  InputParameters params = MooseApp::validParams();

  // Do not use legacy material output, i.e., output properties on INITIAL as well as TIMESTEP_END
  params.set<bool>("use_legacy_material_output") = false;

  return params;
}

porous_modelApp::porous_modelApp(InputParameters parameters) : MooseApp(parameters)
{
  porous_modelApp::registerAll(_factory, _action_factory, _syntax);
}

porous_modelApp::~porous_modelApp() {}

void
porous_modelApp::registerAll(Factory & f, ActionFactory & af, Syntax & syntax)
{
  ModulesApp::registerAll(f, af, syntax);
  Registry::registerObjectsTo(f, {"porous_modelApp"});
  Registry::registerActionsTo(af, {"porous_modelApp"});

  /* register custom execute flags, action syntax, etc. here */
}

void
porous_modelApp::registerApps()
{
  registerApp(porous_modelApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
extern "C" void
porous_modelApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  porous_modelApp::registerAll(f, af, s);
}
extern "C" void
porous_modelApp__registerApps()
{
  porous_modelApp::registerApps();
}
