[Mesh]
	type = GeneratedMesh
	dim = 2
	nx = 20
	ny = 20
[]

[Variables]
	[pressure]
	[]
	[temperature]
		initial_condition = 0
	[]
[]

[AuxVariables]
	[velocity]
		order = CONSTANT
		family = MONOMIAL_VEC
	[]
[]

[Kerenls]
	[darcy_pressure]
		type = DarcyPressure
		variable = pressure
	[]
	[heat_conduction]
		type = ADHeatConduction
		variable = temperature
	[]
	[heat_conduction_time_derivative]
		type = ADHeatConductionTimeDerivative
		variable = temperature
	[]
	[heat_convection]
		type = DarcyAdvection
		variable = temperature
		pressure = pressure
	[]
[]

[AuxKernels]
	[velocity]
		type = DarcyVelocity
		variable = velocity
		execute_on = timestep_end
		pressure = pressure
	[]
[]

[Functions]
	[inlet_function]
		type = ParsedFunction
		value = 2000*sin(0.466*pi*t)
	[]
	[outlet_function]
		type = ParsedFunction
		value = 2000*cos(0.466*pi*t)
	[]
[]

[BCs]
	[inlet]
		type = FunctionDirichletBC
		variable = pressure
		boundary = left
		function = inlet_function
	[]
	[outlet]
		type = FunctionDirichletBC
		variable = pressure
		boundary = right
		function = outlet_function
	[]
	[inlet_temperature]
		type =FunctionDirichletBC
		variable = temperature
		boundary = left
		function = 'if(t<0,350+50*t,350)'
	[]
	[outlet_temperature]
		type = HeatConductionOutflow
		variable = temeprature
		boundary = right
	[]
[]

[Materials]
	[column]
		type = PackedColumn
		diameter = 1
		temperature = temperature
		fluid_viscosity_file = ~/projects/porous_model/steps/data/water_viscosity.csv
		fluid_density_file = ~/projects/porous_model/steps/data/water_density.csv
		fluid_thermal_conductivity_file = ~/projects/porous_model/steps/data/water_thermal_conductivity.csv
		fluid_specific_heat_file = ~/projects/porous_model/steps/data/water_specific_heat.csv
		outputs = exodus
	[]
[]

[Problem]
	type = FEProblem
	coord_type = RZ
	rz_coord_axis = X
[]

[Executioner]
	type = Transient
	solve_type = NEWTOn
	automatic_scaling = true
	
	petsc_options_iname = '-pc_type -pc_hypre_type'
	petsc_options_value = 'hypre boomeramg'
	
	end_time = 100
	dt = 0.25
	start_time = -1
	
	steady_state_tolerance = 1e-5
	steady_state_detection = true
	
	[TimeStepper]
		type = FunctionDT
		function = 'if(t<0,0.1,(2*pi/(0.466*pi))/16)'
	[]
[]
	
