[Mesh]
	type = GeneratedMesh #Can generate simple lines,rectangles and rectangular prisms
	dim  = 2 #Dimensions of the mesh
	nx   = 1000 #Number of elements in the x direction
	ny   = 10   #Number of elements in the y direction
	xmax = 0.304 #Length of test chamber
	ymax = 0.0257 #Test chamber radius
[]

[Problem]
	type = FEProblem #This is the "normal" type of Finite element Problem in MOOSE
	coord_type = RZ #Axis symmetric RZ
	rz_coord_axis = X #Which axis the symmetry is around
[]

[Variables]
	[pressure]
	#Adds a Linear Lagrange variable by default
	[]
	[temperature]
		initial_condition = 300
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
[]

[AuxVariables]
  [velocity]
    order = CONSTANT # Since "pressure" is approximated linearly, its gradient must be constant
    family = MONOMIAL_VEC # A monomial interpolation means this is an elemental AuxVariable
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

[Materials]
	[filter]
		type = PackedColumn #Provides permeability and viscosity of water through packed 1mm spheres
		diameter = '1+2/3.04*x'
		outputs = exodus
	[]
	[steel]
		type = ADGenericConstantMaterial
		prop_names = 'thermal_conductivity specific_heat density'
		prop_values = '18 0.466 8000' 
	[]
[]


[BCs]
  [inlet]
	type = ADDirichletBC
	variable = pressure
	boundary = left
	value = 4000 # (Pa) Gives the correct pressure drop from figure 2 for 1 mm spheres
  []
  [inlet_temperature]
  	type = DirichletBC
  	variable = temperature
  	boundary = left
  	value = 350
  []
  [outlet]
  	type = ADDirichletBC
  	variable = pressure
  	boundary = right
  	value = 0 # (Pa) Gives the correct pressure drop 
  []
  [outlet_temp]
  	type = DirichletBC
  	variable = temperature
  	boundary = right
  	value = 300
  []
 []
 
 [Executioner]
 	type = Transient
 	num_steps = 10
 	solve_type = NEWTON #Perform a Newton Solver
 	
 	#Set PETSc parameters to optimize solver efficiency
 	petsc_options_iname = '-pc_type -pc_hypre_type' #PETSc option pairs with values below
 	petsc_options_value = 'hypre	boomeramg'
 []
 

 [Outputs]
 	exodus = true #Output Exodus Format
 	perf_graph = true # prints a performance report to the terminal
 []
	

