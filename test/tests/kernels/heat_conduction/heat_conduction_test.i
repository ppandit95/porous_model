[Mesh]
	type = GeneratedMesh
	dim = 2
	nx = 10
	ny = 10
[]

[Variables]
	[temperature]
	[]
[]

[Kernels]
	[heat_conduction]
		type = ADHeatConduction
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
		type = DirichletBC
		variable = temperature
		boundary = right
		value = 0
	[]
[]

[Materials]
	[steel]
		type = ADGenericConstantMaterial
		prop_names = thermal_conductivity
		prop_values = 18
	[]
[]
	
[Problem]
	type = FEProblem
	coord_type = RZ
	rz_coord_axis = X
[]

[Executioner]
	type = Steady
	solve_type = NEWTON
	petsc_options_iname = '-pc_type -pc_hypre_type'
	petsc_options_value = 'hypre boomeramg'
[]

[Outputs]
	exodus = true
[]
