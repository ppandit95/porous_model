#include "DarcyAdvection.h"

registerMooseObject("porous_modelApp",DarcyAdvection);

InputParameters
DarcyAdvection::validParams()
{
	InputParameters params = ADKernelValue::validParams();
	params.addRequiredCoupledVar("pressure","The variable representing the pressure.");
	return params;
}

DarcyAdvection::DarcyAdvection(const InputParameters & parameters)
 : ADKernelValue(parameters),
 //Couple to the gradient of the pressure
 _pressure_grad(adCoupledGradient("pressure")),
 //Grab necessary material properties
 _permeability(getADMaterialProperty<Real>("permeability")),
 _porosity(getADMaterialProperty<Real>("porosity")),
 _viscosity(getADMaterialProperty<Real>("viscosity")),
 _density(getADMaterialProperty<Real>("density")),
 _specific_heat(getADMaterialProperty<Real>("specific_heat"))
 {}
 
 ADReal
 DarcyAdvection::precomputeQpResidual()
 {
 	ADRealVectorValue superficial_velocity = 
 		_porosity[_qp] * -(_permeability[_qp]/_viscosity[_qp]) * _pressure_grad[_qp];
 	
 	return _density[_qp] * _specific_heat[_qp] * superficial_velocity * _grad_u[_qp];
 }
