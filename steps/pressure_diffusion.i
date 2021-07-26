[Mesh]
	type = GeneratedMesh #Can generate simple lines,rectangles and rectangular prisms
	dim  = 2 #Dimensions of the mesh
	nx   = 30 #Number of elements in the x direction
	ny   = 3   #Number of elements in the y direction
	xmax = 0.304 #Length of test chamber
	ymax = 0.0257 #Test chamber radius
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
	[filter]
		type = PackedColumn #Provides permeability and viscosity of water through packed 1mm spheres
		diameter = 1
		temperature = temperature
		fluid_viscosity_file = data/water_viscosity.csv
		fluid_density_file = data/water_density.csv
		fluid_thermal_conductivity_file = data/water_thermal_conductivity.csv
		fluid_specific_heat_file = data/water_specific_heat.csv
		outputs = exodus
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
 

 [Outputs]
 	exodus = true #Output Exodus Format
 	perf_graph = true # prints a performance report to the terminal
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
