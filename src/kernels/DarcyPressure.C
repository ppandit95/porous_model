#include "DarcyPressure.h"

registerMooseObject("porous_modelApp",DarcyPressure);

InputParameters 
DarcyPressure::validParams(){
	InputParameters params = ADKernelGrad::validParams();
	params.addClassDescription("Compute the diffusion term for Darcy Pressure($p$) equation :"
				    "$-\\nabla \\cdot \\frac{\\mathbf{K}}{\\mu}\\nabla p = 0$");
				    
	return params;
}

DarcyPressure::DarcyPressure(const InputParameters & parameters):ADKernelGrad(parameters),
//Set the coefficients for the pressure kernel
_permeability(getADMaterialProperty<Real>("permeability")),
_viscosity(getADMaterialProperty<Real>("viscosity"))
{
}

ADRealVectorValue
DarcyPressure::precomputeQpResidual(){
	return (_permeability[_qp]/_viscosity[_qp])*_grad_u[_qp];
}
