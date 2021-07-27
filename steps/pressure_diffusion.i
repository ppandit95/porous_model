[Mesh]
	[generate]
		type = GeneratedMeshGenerator #Can generate simple lines,rectangles and rectangular prisms
		dim  = 2 #Dimensions of the mesh
		nx   = 30 #Number of elements in the x direction
		ny   = 3   #Number of elements in the y direction
		xmax = 0.304 #Length of test chamber
		ymax = 0.0257 #Test chamber radius
	[]
	[bottom]
		type = SubdomainBoundingBoxGenerator
		input = generate
		location = inside
		bottom_left = '0 0 0'
		top_right = '0.304 0.01285 0'
		block_id = 1
	[]
[]



[Variables]
	[pressure]
	#Adds a Linear Lagrange variable by default
	[]
	[temperature]
		initial_condition = 300
	[]
[]

[AuxVariables]
  [velocity]
    order = CONSTANT # Since "pressure" is approximated linearly, its gradient must be constant
    family = MONOMIAL_VEC # A monomial interpolation means this is an elemental AuxVariable
  []
[]

[Kernels]
	[diffusion]
		type = DarcyPressure # Zero gravity,divergence-free form of Darcy's Law
		variable = pressure #operate on the "pressure" variable from above
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
    variable = velocity # Store volumetric flux vector in "velocity" variable from above
    pressure = pressure # Couple to the "pressure" variable from above
    execute_on = TIMESTEP_END # Perform calculation at the end of the solve step - after Kernels run
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
  [inlet_temperature]
  	type = FunctionDirichletBC
  	variable = temperature
  	boundary = left
  	function = 'if(t<0,350+50*t,350)'
  []
  [outlet]
  	type = FunctionDirichletBC
  	variable = pressure
  	boundary = right
  	function = outlet_function
  []
  [outlet_temp]
  	type = HeatConductionOutflow
  	variable = temperature
  	boundary = right
  	
  []
 []
 

[Materials]
	viscosity_file = data/water_viscosity.csv
	density_file = data/water_density.csv
	thermal_conductivity_file = data/water_thermal_conductivity.csv
	specific_heat_file = data/water_specific_heat.csv
	[colum_bottom]
		type = PackedColumn #Provides permeability and viscosity of water through packed 1mm spheres
		block = 1
		diameter = 1.15
		temperature = temperature
		fluid_viscosity_file = ${viscosity_file}
		fluid_density_file = ${density_file}
		fluid_thermal_conductivity_file = ${thermal_conductivity_file}
		fluid_specific_heat_file = ${specific_heat_file}
	[]
	[column_top]
		type = PackedColumn
		block = 0
		diameter = 1.45
		temperature = temperature
		porosity = '0.25952 + 0.7*x/0.304'
		fluid_viscosity_file = ${viscosity_file}
		fluid_density_file = ${density_file}
		fluid_thermal_conductivity_file = ${thermal_conductivity_file}
		fluid_specific_heat_file = ${specific_heat_file}
	[]	
[]


[Problem]
	type = FEProblem #This is the "normal" type of Finite element Problem in MOOSE
	coord_type = RZ #Axis symmetric RZ
	rz_coord_axis = X #Which axis the symmetry is around
[]

 [Executioner]
 	type = Transient
 	solve_type = NEWTON #Perform a Newton Solver
 	automatic_scaling = true
 	
 	#Set PETSc parameters to optimize solver efficiency
 	petsc_options_iname = '-pc_type -pc_hypre_type' #PETSc option pairs with values below
 	petsc_options_value = 'hypre	boomeramg'
 	
 	end_time = 10
 	dt = 0.25
 	start_time = -1
 	
 	steady_state_tolerance = 1e-5
 	steady_state_detection = true
 	
 	[TimeStepper]
 		type = FunctionDT
 		function = 'if(t<0,0.1,(2*pi/(0.466*pi))/16)'
 	[]
 []
 
 [Postprocessors]
 	[average_temperature]
 		type = ElementAverageValue
 		variable = temperature
 	[]
 	[outlet_heat_flux]
 		type = ADSideDiffusiveFluxIntegral
 		variable = temperature
 		boundary = right
 		diffusivity = thermal_conductivity
 	[]
 []
 
 [VectorPostprocessors]
 	[temperature_sample]
 		type = LineValueSampler
 		num_points = 500
 		start_point = '0.1 0 0'
 		end_point = '0.1 0.0257 0'
 		variable = temperature
 		sort_by = y
 	[]
 []

 [Outputs]
 	exodus = true #Output Exodus Format
 	perf_graph = true # prints a performance report to the terminal
 	output_material_properties = true
 []
 [Adaptivity]
 	marker = error_frac
 	max_h_level = 3
 	[Indicators]
 		[temperature_jump]
 			type = GradientJumpIndicator
 			variable = temperature
 			scale_by_flux_faces = true
 		[]
 	[]
 	[Markers]
 		[error_frac]
 			type = ErrorFractionMarker
 			coarsen = 0.15
 			indicator = temperature_jump
 			refine = 0.7
 		[]
 	[]
 []
