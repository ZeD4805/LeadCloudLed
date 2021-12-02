#Led driving steps

### Steps:
- [x] Map registers to memory
	- [x] Map Base GPIO register
	- [x] Map CLK register
	- [x] Map PWM register
- [x] Set pins to Output
- [ ] Drive OE pin with PWM for dimming
- [ ] Quickly fill Output registers GPSET0 / GPCLR0
	- Suggestion: create a virtual placement of the pins, fill that and simply copy to the register


Driving steps: See Framebuffer::DumpToMatrix in framebuffer.cc

- 1 PWM pulse per bit of color

		-Fill color of pixel and reset clock
		-set clock (rising edge)
	-clear clock
	-Wait for the previous PWM pulse to finish
	
	-Change rown address
	
	-Set STB
	-Clear STB

	-start a new PWM pulse
	
	



