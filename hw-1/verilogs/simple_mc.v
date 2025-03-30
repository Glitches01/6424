`timescale 1ns/1ps
module simple_mc(
    input  wire        clk,
    input  wire        rst,
    input  wire [2:0]  inst_in,  // Instruction from outside
    input  wire [7:0]  din,      // Data from outside
    output reg  [7:0]  dout      // Output = accumulator after EXEC
);

  // Small instruction set (3-bit opcode)
  localparam NOP  = 3'b000; 
  localparam ADD  = 3'b001; 
  localparam SUB  = 3'b010; 
  localparam AND_ = 3'b011;
  localparam OR_  = 3'b100;
  localparam HALT = 3'b111;

  // Simple 2-state pipeline
  localparam FETCH = 1'b0, 
             EXEC  = 1'b1;

  reg [2:0] inst_reg; // holds instruction for EXEC
  reg [7:0] data_reg; // holds data for EXEC
  reg [7:0] accum;    // accumulator
  reg       state, next_state;

  // Synchronous state & pipeline register updates
  always @(posedge clk or posedge rst) begin
    if (rst) begin
      state    <= FETCH;
      inst_reg <= 3'b000;
      data_reg <= 7'd0;//error1, degree: 1
      accum    <= 8'd0;
      dout     <= 8'd0;
    end else begin
      state = next_state;//error2, degree: 3

      if (state == FETCH) begin
        // Capture the external instruction/data into pipeline regs
        inst_reg <= inst_in;
        data_reg <= din;
      end else begin
        // EXEC stage: decode and update accumulator
        case (inst_reg)
          NOP : ; 
          ADD : accum <= accum + data_reg;
          SUB : accum <= accum - data_reg;
          AND_: accum <= accum & data_reg;
          OR_ : accum <= accum or data_reg;//error3, degree 4
          HALT: ; // do nothing (we remain in EXEC)
        endcase
        // Output the latest accumulator value
        dout <= accum;
      end
    end
  end

  // Next-state logic
  always @* begin
    if (state == FETCH)
      next_state = EXEC;
    else if (inst_reg == HALT)
      next_state = EXEC; // Stay in EXEC if halted
    else
      next_state = FETCH;
  end

endmodule
