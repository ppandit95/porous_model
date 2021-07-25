[Mesh]
	type = GeneratedMesh
	dim = 2
	nx = 10
	ny = 10
[]

[Variables]
	[temperature]
		initial_condition = 0
	[]
[]

[Kernels]
	[heat_conduction]
		type = ADHeatConduction
		variable = temperature
	[]
	[heat_conduction_time_derivative]
		type = ADHeatConductionTimeDerivative
		variable = temperature
	[]
[]

[BCs]
	[inlet]
		type = DirichletBC
		variable = temperature
		boundary = left
		value = 1 #
	[]
	[outlet]
		type = HeatConductionOutflow
		variable = temperature
		boundary = right
		
	[]
[]

[Materials]
	[steel]
		type = ADGenericConstantMaterial
		prop_names = 'thermal_conductivity specific_heat density'
		prop_values = '18 0.466 8000'
	[]
[]
	
[Problem]
	type = FEProblem
	coord_type = RZ
	rz_coord_axis = X
[]

[Executioner]
	type = Transient
	num_steps = 10
	solve_type = NEWTON
	petsc_options_iname = '-pc_type -pc_hypre_type'
	petsc_options_value = 'hypre boomeramg'
[]

[Outputs]
	exodus = true
[]
