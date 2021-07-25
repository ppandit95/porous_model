#pragma once

#include "ADKernelValue.h"

/**
 * Kernel which implements the convective term in the transient heat conduction equation,and provides coupling with the Darcy Pressure Equation.
 */
 
 class DarcyAdvection : public ADKernelValue
 {
 	public:
 		static InputParameters validParams();
 		
 		DarcyAdvection(const InputParameters & parameters);
 		
 	protected:
 		//ADKernelValue objects must override precomputeQpResidual
 		virtual ADReal precomputeQpResidual() override;
 		
 		//The gradient of pressure
 		const ADVariableGradient & _pressure_grad;
 		
 		//These references will be set by the initialization list so that
 		// value can be pulled from the Material System
 		const ADMaterialProperty<Real> & _permeability;
 		const ADMaterialProperty<Real> & _porosity;
 		const ADMaterialProperty<Real> & _viscosity;
 		const ADMaterialProperty<Real> & _density;
 		const ADMaterialProperty<Real> & _specific_heat;
 };
