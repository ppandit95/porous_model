#pragma once 

#include  "ADMaterial.h"

//A helper class from MOOSE that linearly interpolates abscissa - ordinate pairs
#include "LinearInterpolation.h"

/**
 * Computes the permeability of a porous medium made up of packed steel spheres of a specified
 * diameter in accordance with Pamuk and Ozdemir (2012) .This  also provides a specified dynamic
 * viscosity of the fluid in the medium.
 */
 
class PackedColumn : public ADMaterial
{
	public:
	  static InputParameters validParams();
	  
	  PackedColumn(const InputParameters & parameters);
	  
	 protected:
	   //Necessary override. This is where the property values are set.
	   virtual void computeQpProperties() override;
	   
	   /**
	    * Helper function to read CSV data for use in an interpolator object.
	    */
	    bool initInputData(const std::string & param_name,ADLinearInterpolation & interp);
	   
	   //The inputs for the diameter of spheres in the column and the dynamic viscosity of the  
	   const Function & _diameter;
	   
	   //The input porosity
	   const Function & _input_porosity;
	   
	   //Temperature
	   const ADVariableValue & _temperature;
	   
	   //This object interpolates permeability (m^2) based on the diameter(mm)
	   LinearInterpolation _permeability_interpolation;
	   
	   //Fluid Viscosity
	   bool _use_fluid_mu_interp;
	   const Real & _fluid_mu;
	   ADLinearInterpolation _fluid_mu_interpolation;
	   
	   
	   //Fluid Thermal Conductivity
	   bool _use_fluid_k_interp = false;
	   const Real & _fluid_k;
	   ADLinearInterpolation _fluid_k_interpolation;
	   
	   //Fluid Density
	   bool _use_fluid_rho_interp = false;
	   const Real & _fluid_rho;
	   ADLinearInterpolation _fluid_rho_interpolation;
	   
	   //Fluid specific Heat
	   bool _use_fluid_cp_interp;
	   const Real & _fluid_cp;
	   ADLinearInterpolation _fluid_cp_interpolation;
	   
	   //Solid thermal conductivity
	   bool _use_solid_k_interp = false;
	   const Real & _solid_k;
	   ADLinearInterpolation _solid_k_interpolation;
	   
	   //Solid Density
	   bool _use_solid_rho_interp = false;
	   const Real & _solid_rho;
	   ADLinearInterpolation _solid_rho_interpolation;
	   
	   //Solid Specific Heat
	   bool _use_solid_cp_interp;
	   const Real & _solid_cp;
	   ADLinearInterpolation _solid_cp_interpolation;
	   
	   
	   
	   
	   //The material property objects that hold values for permeability (K) and dynamic viscosity (mu)
	   ADMaterialProperty<Real> & _permeability;
	   ADMaterialProperty<Real> & _viscosity;
	   ADMaterialProperty<Real> & _porosity;
	   ADMaterialProperty<Real> & _thermal_conductivity;
	   ADMaterialProperty<Real> & _density;
	   ADMaterialProperty<Real> & _specific_heat;
};
	   
