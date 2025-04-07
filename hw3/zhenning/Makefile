%.o: %.c
	gcc -c $<

%.f: %.o
	gcc $< -o $@ -ldl

%.run: %.f
	./$<

spectre: spectre.c
	gcc -O2 -o spectre spectre.c
r: spectre
	./$<
.INTERMEDIATE: %.o %.f

clean:
	rm -rf ./*.o ./*.f
