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
[]

[Kernels]
	[diffusion]
	type = ADDiffusion # laplacian Operator
	variable = pressure #operate on the "pressure" variable from above
	[]
[]

[BCs]
  [inlet]
	type = ADDirichletBC
	variable = pressure
	boundary = left
	value = 4000 # (Pa) Gives the correct pressure drop from figure 2 for 1 mm spheres
  []

  [outlet]
  	type = ADDirichletBC
  	variable = pressure
  	boundary = right
  	value = 0 # (Pa) Gives the correct pressure drop 
  []
 []
 
 [Executioner]
 	type = Steady #Steady state problem
 	solve_type = NEWTON #Perform a Newton Solver
 	
 	#Set PETSc parameters to optimize solver efficiency
 	petsc_options_iname = '-pc_type -pc_hypre_type' #PETSc option pairs with values below
 	petsc_options_value = 'hypre	boomerang'
 []
 
 [Outputs]
 	exodus = true #Output Exodus Format
 []
	
