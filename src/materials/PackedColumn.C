#include "PackedColumn.h"
#include "Function.h"
#include "DelimitedFileReader.h"

registerMooseObject("porous_modelApp",PackedColumn);

InputParameters
PackedColumn::validParams()
{
	InputParameters params = ADMaterial::validParams();
	params.addClassDescription("Computes the permeability of a porous medium made up of packed"
				    "steel spheres of a specified diameter in accordance with Pamuk and"
				    "Ozdemir(2012). This also provides a specified dynamic viscosity of"
				    "the fluid in the medium.");
	params.addRequiredCoupledVar("temperature","The temperature (C) of the fluid");
				    
	//Optional params for ball diameter and viscosity - inputs must satisfy range checked conditions
	params.addParam<FunctionName>("diameter","1.0","The diameter of the steel spheres"
	 "				that are packed in the column for computing permeability.");
	
	params.addParam<FunctionName>("porosity",0.25952,
					"Porosity of porous media,default is for close packed sphere");
					
	//Fluid Properties
	params.addParam<Real>("fluid_viscosity",1.002e-3,"Fluid viscosity ;default is for water at 20C");
	params.addParam<FileName>("fluid_viscosity_file","The name of a file containing the fluid viscosity as a function of temperature(C);if provided the constant value is ignored");
	
	params.addParam<Real>("fluid_thermal_conductivity",0.59803,"Fluid Thermal Conductivity (W/(mK); default is for water at 20C)");
	params.addParam<FileName>("fluid_thermal_conductivity_file","The name of a file containing fluid thermal conductivity (W/(mK)) as a function of temperature (C);if provided the constant value is ignored.");
	
	params.addParam<Real>("fluid_density",998.21,"Fluid Density (kg/m^3);default is for water at 20C");
	params.addParam<FileName>("fluid_density_file","The name of a file containing fluid density (kg/m^3) as a function of temperature (C);if provided the constant value is ignored.");
	
	params.addParam<Real>("fluid_specific_heat",4157.0,"Fluid Specific Heat (J/(kgK));default is for water at 20C");
	params.addParam<FileName>("fluid_specific_heat_file","The name of a file containing fluid Specific Heat (J/(kgK))as a function of temperature (C);if provided the constant value is ignored.");
	
	params.addParam<Real>("fluid_thermal_expansion",2.07e-4,"Fluid thermal expansion coefficient (1/K); default is for water at 20C).");
  	params.addParam<FileName>("fluid_thermal_expansion_file","The name of a file containing fluid thermal expansion coefficient (1/K) as a function of temperature (C); if provided the constant value is ignored.");
	
	//Solid Properties
	params.addParam<Real>("solid_thermal_conductivity",15.0,"Solid Thermal Conductivity (W/(mK); default is for steel at 20C)");
	params.addParam<FileName>("solid_thermal_conductivity_file","The name of a file containing solid thermal conductivity (W/(mK)) as a function of temperature (C);if provided the constant value is ignored.");
	
	params.addParam<Real>("solid_density",7900,"Solid Density (kg/m^3);default is for steel at 20C");
	params.addParam<FileName>("solid_density_file","The name of a file containing solid density (kg/m^3) as a function of temperature (C);if provided the constant value is ignored.");
	
	params.addParam<Real>("solid_specific_heat",500,"Solid Specific Heat (J/(kgK));default is for steel at 20C");
	params.addParam<FileName>("solid_specific_heat_file","The name of a file containing solid Specific Heat (J/(kgK))as a function of temperature (C);if provided the constant value is ignored.");
	
	params.addParam<Real>("solid_thermal_expansion",17.3e-6,"Solid thermal expansion coefficient (1/K); default is for water at 20C).");
  	params.addParam<FileName>("solid_thermal_expansion_file","The name of a file containing solid thermal expansion coefficient (1/K) as a function of temperature (C); if provided the constant value is ignored.");
	
	return params;
}

PackedColumn::PackedColumn(const InputParameters & parameters)
:Material(parameters),
_diameter(getFunction("diameter")),
_input_porosity(getFunction("porosity")),
_temperature(adCoupledValue("temperature")),

//Fluid Properties
_fluid_mu(getParam<Real>("fluid_viscosity")),
_fluid_k(getParam<Real>("fluid_thermal_conductivity")),
_fluid_rho(getParam<Real>("fluid_density")),
_fluid_cp(getParam<Real>("fluid_specific_heat")),
    _fluid_cte(getParam<Real>("fluid_thermal_expansion")),

//Solid Properties
_solid_k(getParam<Real>("solid_thermal_conductivity")),
_solid_rho(getParam<Real>("solid_density")),
_solid_cp(getParam<Real>("solid_specific_heat")),
    _solid_cte(getParam<Real>("solid_thermal_expansion")),

//Declare material properties by getting a reference from the MOOSE Material system
_permeability(declareADProperty<Real>("permeability")),
_porosity(declareADProperty<Real>("porosity")),
_viscosity(declareADProperty<Real>("viscosity")),
_thermal_conductivity(declareADProperty<Real>("thermal_conductivity")),
_specific_heat(declareADProperty<Real>("specific_heat")),
_density(declareADProperty<Real>("density")),
_thermal_expansion(declareADProperty<Real>("thermal_expansion"))
{
	//Set Data for Permeability
	std::vector<Real> sphere_sizes = {1,3};
	std::vector<Real> permeability = {0.8451e-9,8.968e-9};
	_permeability_interpolation.setData(sphere_sizes,permeability);
	
	//Fluid viscosity,thermal conductivity,density and specific heat
	_use_fluid_mu_interp = initInputData("fluid_viscosity_file",_fluid_mu_interpolation);
	_use_fluid_k_interp = initInputData("fluid_thermal_conductivity_file",_fluid_k_interpolation);
	_use_fluid_rho_interp = initInputData("fluid_density_file",_fluid_rho_interpolation);
	_use_fluid_cp_interp = initInputData("fluid_specific_heat_file",_fluid_cp_interpolation);
	_use_fluid_cte_interp = initInputData("fluid_thermal_expansion_file", _fluid_cte_interpolation);
	
	//Solid thermal conductivity,density and specific heat
	_use_solid_k_interp = initInputData("solid_thermal_conductivity_file",_solid_k_interpolation);
	_use_solid_rho_interp = initInputData("solid_density_file",_solid_rho_interpolation);
	_use_solid_cp_interp = initInputData("solid_specific_heat_file",_solid_cp_interpolation);
	_use_solid_cte_interp = initInputData("solid_thermal_expansion_file", _solid_cte_interpolation);
}

void 
PackedColumn::computeQpProperties()
{
	//Current Temperature
	ADReal temp = _temperature[_qp] - 273.15;
	//Permeability
	Real value = _diameter.value(_t,_q_point[_qp]);
	mooseAssert(value >= 1 && value <= 3,
		    "The diameter range must be in the range [1,3] but"<<value<<"provided.");
	_permeability[_qp] = _permeability_interpolation.sample(value);
	
	//Porosity
	Real porosity_value = _input_porosity.value(_t,_q_point[_qp]);
	mooseAssert(porosity_value > 0 && porosity_value <= 1,
		     "The porosity range must be in the range (0,1] but"<<porosity_value<<" provided.");
 	_porosity[_qp] = porosity_value;
 	
 	//Fluid Properties
 	_viscosity[_qp] = _use_fluid_mu_interp ? _fluid_mu_interpolation.sample(temp) : _fluid_mu;
 	ADReal fluid_k = _use_fluid_k_interp ? _fluid_k_interpolation.sample(temp) : _fluid_k;
 	ADReal fluid_rho = _use_fluid_rho_interp ? _fluid_rho_interpolation.sample(temp) : _fluid_rho;
 	ADReal fluid_cp = _use_fluid_cp_interp ? _fluid_cp_interpolation.sample(temp) : _fluid_cp;
 	ADReal fluid_cte = _use_fluid_cte_interp ? _fluid_cte_interpolation.sample(temp) : _fluid_cte;
 	
 	//Solid Properties
 	ADReal solid_k = _use_solid_k_interp ? _solid_k_interpolation.sample(temp) : _solid_k;
 	ADReal solid_rho = _use_solid_rho_interp ? _solid_rho_interpolation.sample(temp) : _solid_rho;
 	ADReal solid_cp = _use_solid_cp_interp ? _solid_cp_interpolation.sample(temp) : _solid_cp;
 	ADReal solid_cte = _use_solid_cte_interp ? _solid_cte_interpolation.sample(temp) : _solid_cte;
 	
 	//Compute the heat conduction material properties as a linear combination of the fluid and steel
 	_thermal_conductivity[_qp] = _porosity[_qp] * fluid_k + (1.0 - _porosity[_qp])*solid_k;
 	_density[_qp] =  _porosity[_qp] * fluid_rho + (1.0 - _porosity[_qp])*solid_rho;
 	_specific_heat[_qp] =  _porosity[_qp] * fluid_cp + (1.0 - _porosity[_qp])*solid_cp;
 	_thermal_expansion[_qp] = _porosity[_qp] * fluid_cte + (1.0 - _porosity[_qp]) * solid_cte;
}

bool 
PackedColumn::initInputData(const std::string & param_name,ADLinearInterpolation & interp)
{
	if(isParamValid(param_name))
	{
		const std::string & filename = getParam<FileName>(param_name);
		MooseUtils::DelimitedFileReader reader(filename, & _communicator);
		reader.setComment("#");
		reader.read();
		interp.setData(reader.getData(0),reader.getData(1));
		return true;
	}
	return false;
}
