//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
#include "porous_modelTestApp.h"
#include "porous_modelApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "MooseSyntax.h"
#include "ModulesApp.h"

InputParameters
porous_modelTestApp::validParams()
{
  InputParameters params = porous_modelApp::validParams();
  return params;
}

porous_modelTestApp::porous_modelTestApp(InputParameters parameters) : MooseApp(parameters)
{
  porous_modelTestApp::registerAll(
      _factory, _action_factory, _syntax, getParam<bool>("allow_test_objects"));
}

porous_modelTestApp::~porous_modelTestApp() {}

void
porous_modelTestApp::registerAll(Factory & f, ActionFactory & af, Syntax & s, bool use_test_objs)
{
  porous_modelApp::registerAll(f, af, s);
  if (use_test_objs)
  {
    Registry::registerObjectsTo(f, {"porous_modelTestApp"});
    Registry::registerActionsTo(af, {"porous_modelTestApp"});
  }
}

void
porous_modelTestApp::registerApps()
{
  registerApp(porous_modelApp);
  registerApp(porous_modelTestApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
// External entry point for dynamic application loading
extern "C" void
porous_modelTestApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  porous_modelTestApp::registerAll(f, af, s);
}
extern "C" void
porous_modelTestApp__registerApps()
{
  porous_modelTestApp::registerApps();
}
