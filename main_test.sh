rm -rf obj_dir/

verilator --cc \
  main.sv \
  program_counter.sv \
  instruction_memory.sv \
  control_unit.sv \
  register_file.sv \
  alu.sv \
  immediate_extend.sv \
  --exe main_test.cpp sCPU.cpp \
  --trace

make -C obj_dir -f Vmain.mk

./obj_dir/Vmain

# gtkwave waveform_cpu.vcd

echo "Simulation complete. Waveform saved to waveform_cpu.vcd"
echo "You can view it using GTKWave: gtkwave waveform_cpu.vcd"