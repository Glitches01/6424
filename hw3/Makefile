%.o: %.c
	g++ -c $<

%.f: %.o
	g++ $< -o $@

%.run: %.f
	./$<

.INTERMEDIATE: %.o %.f

clean:
	rm -rf ./*.o
