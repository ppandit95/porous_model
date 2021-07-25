#pragma once

//Include the base class so it can be extended
#include "ADIntegratedBC.h"

/**
 * An IntegratedBC representing the "No BC" boundary condition for heat conduction
 *
 * The residual is simply -test*k*grad_u*normal.. the term which appears after integration by parts.
 * This is a standard technique for truncating longer domains when solving convection/diffusion 
 * equation.
 */
 
class HeatConductionOutflow : public ADIntegratedBC
{
	public:
		static InputParameters validParams();
		
		HeatConductionOutflow(const InputParameters & parameters);
		
	protected:
	/**
	 * This is called to integrate the residual across boundary
	 */
	 
	 virtual ADReal computeQpResidual() override;
	 
	 //Thermal Conductivity of the material
	 const ADMaterialProperty<Real> & _thermal_conductivity;
};
	 
