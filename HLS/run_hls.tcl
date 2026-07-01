# Vivado HLS batch script for sobel_accel
# Run from the workspace root with vivado_hls -f run_hls.tcl

set project_name sobel_hls
set solution_name solution1
set top_function sobel_accel
set part_name xc7z020clg400-1  ;# change to your target device if needed

if {![file exists $project_name]} {
    create_project $project_name . -part $part_name
} else {
    open_project $project_name
}

set_top $top_function
add_files sobel_accel.cpp sobel.h
open_solution $solution_name
set_part $part_name
csynth_design
report_utilization -hierarchical
report_timing -delay_type max
export_design -format ip_catalog
exit
