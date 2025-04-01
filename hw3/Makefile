%.o: %.c
	g++ -c $<

%.f: %.o
	g++ $< -o $@ -ldl

%.run: %.f
	./$<

.INTERMEDIATE: %.o %.f

clean:
	rm -rf ./*.o ./*.f
