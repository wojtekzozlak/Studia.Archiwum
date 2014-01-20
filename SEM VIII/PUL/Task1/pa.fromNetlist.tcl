
# PlanAhead Launch Script for Post-Synthesis floorplanning, created by Project Navigator

create_project -name Task1 -dir "E:/VHDL/Task1/planAhead_run_2" -part xc3s250ecp132-4
set_property design_mode GateLvl [get_property srcset [current_run -impl]]
set_property edif_top_file "E:/VHDL/Task1/main.ngc" [ get_property srcset [ current_run ] ]
add_files -norecurse { {E:/VHDL/Task1} }
set_property target_constrs_file "Basys2iodef.ucf" [current_fileset -constrset]
add_files [list {Basys2iodef.ucf}] -fileset [get_property constrset [current_run]]
link_design
