# BSQ
[42 C piscine](https://42.fr/en/admissions/42-piscine/) final project  
will you find the biggest square??
## problem definition
the aim of this project is to find the biggest square on a map, avoiding obstacles.  
>given map
```
10.ox
. . . . . . . . o . . o . . . . . . . . . .
o o . . . . . o . . . . . . . . . . . . . o
. . . . . o . . . . . . . . . . . o . . . .
. . . . . . . . . . . . o . . o . . . . . .
. . o . . . . . . . . . . . . . . . . . . .
. . . . . . . . . . . . . . . o . . . . o .
. . o . . . . . . . . . o . . . . . . . . .
. . . . . . . . . . . . . . . . . . . . . .
. . . . . o . . . . . . . . . . . . . . . .
. . . . . . . . . . o . . . . . . . . . . .
```
```
. . . . . . . . o . . o . . . . . . . . . .
o o . . . . . o . . . . . . . . . . . . . o
. . . . . o x x x x x x . . . . . o . . . .
. . . . . . x x x x x x o . . o . . . . . .
. . o . . . x x x x x x . . . . . . . . . .
. . . . . . x x x x x x . . . o . . . . o .
. . o . . . x x x x x x o . . . . . . . . .
. . . . . . x x x x x x . . . . . . . . . .
. . . . . o . . . . . . . . . . . . . . . .
. . . . . . . . . . o . . . . . . . . . . .
```
>after finding the biggest square!!  

the first line of the map (`10.ox`) tells us the number of lines, the `empty`, `obstacle` and `square` characters.  
Invalid maps (different line lengths, invalid or duplicate characters, ...) must print `map error` on the error output (`stderr`).  
<sub>The maps in this readme are spaced out to render an actual square. The maps we read do not include spaces, and by default the program prints the map exactly as it was given, as the subject requires. `make spaced` builds a version that prints it spaced out, like the pictures here.</sub>

## constraints: limited functions and [42 norminette](https://github.com/42School/norminette/blob/master/pdf/en.norm.pdf) compliant
`open`, `close`, `read`, `write`, `malloc`, `free`. that's it!!  
max 5 functions per file, 25 lines per function, 80 columns per line  
max 4 parameters and 5 variable declaration per function  
no "for loops"  
and my own rule: everything fits in 2 `.c` files, so 10 functions at most, `main` included
```
	% norminette src
bsq.h: OK!
bsq_write.c: OK!
bsq_read.c: OK!
	% norminette -v
norminette 3.3.60
```
# why my solution is really cool !!!!
### the algorithm
fill the map with numbers, each cell's value represent the bottom-right corner of the biggest square that fits there.  
```
1 1 1 1 1 1 1 1 0 1 1 0 1 1 1 1 1 1 1 1 1 1 
0 0 1 2 2 2 2 0 1 1 2 1 1 2 2 2 2 2 2 2 2 0 
1 1 1 2 3 0|1 1 1 2 2 2|2 2 3 3 3 0 1 2 3 1 
1 2 2 3 3 1|1 2 2 2 3 3|0 1 2 0 1 1 1 2 3 2 
1 2 0 1 2 2|2 2 3 3 3 4|1 1 2 1 1 2 2 2 3 3 
1 2 1 1 2 3|3 3 3 4 4 4|2 2 2 0 1 2 3 3 0 1 
1 2 0 1 2 3|4 4 4 4 5 5|0 1 2 1 1 2 3 4 1 1 
1 2 1 1 2 3|4 5 5 5 5 6|1 1 2 2 2 2 3 4 2 2 
1 2 2 2 2 0 1 2 3 4 5 6 2 2 2 3 3 3 3 4 3 3 
1 2 3 3 3 1 1 2 3 4 0 1 2 3 3 3 4 4 4 4 4 4 
```
```
1 1 1 1 1 . . . o . . o . . . . . . . . . .
0 0 1 2 2 . . o . . . . . . . . . . . . . o
1 1 1 2 3 o|1 x x x x x|. . . . . o . . . .
1 2 2 2 3 .|x 2 x x x x|o . . o 1 1 | . . .
. . o . . .|x x 3 x x x|. . . . 1 2 | . . .
. . . . . .|x x x 4 x x|. . . o - - 3 . o .
. . o . . .|x x x x 5 x|o . . . . . . . 1 .
. . 1 . . .|x x x x x 6|1 - . . . . . . . 2
. . 2 . . o . . . . . . | 2 | . . . . . . .
. . . . . . . . . . o 1 2 - 3 . . . . . . .
```
6 is the size of the biggest square!!  
<sub>you might notice a second `6` right below the first one, the square also fits there! but we choose the one closest to the top left of the map</sub>  
  
each value is calculated like this:  

* if this cell is an obstacle -> 0  
* if it's on the top or left edge -> 1  
* otherwise, the smallest of its top, left and top-left neighbours, + 1


### the flow
`main` reads like the program runs. for each map:
```c
		map = get_map(fd, &bsq);	//read and solve, NULL if the map is invalid
		close(fd);
		if (map)
			print_square(&bsq);		//the map with the square
		else
			write(2, "map error\n", 10);
```
and `get_map` runs the reading steps, one function each:
```c
	read_rules(fd, bsq);		//y, and the empty, obstacle and full characters
	read_first_line(fd, bsq);	//x
	...							//allocate the bitmap and the row of ints, at their exact size
	read_map(fd, bsq);			//every line: each cell is stored as a bit and solved
```
the map is solved while it's read: each cell goes into the row of ints as soon as it's checked, so there is no second pass over the bitmap, only the one to print it.  
`t_bsq` carries everything between the steps. when a step fails it sets `bsq->err`, and every step after it does nothing, so `get_map` needs no `if` around them, and returns `NULL`.  
`src/bsq_read.c` holds `main` and the reading, `src/bsq_write.c` holds what writes: the bits, the row of ints, the output, and growing the read buffer.

### squeezed tiny memory footprint
for a map of dimensions $x\times y$,  
your first instinct might be to allocate $x\times y$ `char` to store the map and $x\times y$ `int` to calculate the values.  
but we can do much better!!:  
* because we keep track of the map's dimension in a separate `t_bsq` variable, we can store the map in a one-dimension `char` array, which means simpler allocation, saving space that would be needed for multiple pointers, and more efficient accessing.  
* when reading the map, the map itself is allocated exactly once, at its exact size:  
	* the "rules line" is at most 14 characters (10 digits for `INT_MAX`, 3 characters, `\n`), so it's read into a 14 `char` array on the stack, one byte at a time. it's never more than 14 `read`s, and it never reads past the `\n`
	* the first line of the map is read in chunks into a buffer that starts at `BUF_START` (64) bytes and grows by `BUF_GROW` (2) until the first `\n` fits. this is the only buffer that grows, and it's only bytes, so growing it is a simple copy
	* at the end of the first line we know $x$, and $y$ was in the rules line, so we can allocate the exact space for the whole map
	> this is really tricky because $y$ is given, but $x$ is not, until we're done with the whole first line  
	* the rest of the map is read with that same buffer, so each `read` asks for about one line. the number of cells and rows read so far decides where each `\n` must be, so it doesn't matter where a chunk ends, and a line that's too long can never write past the map
* because the map is made of only "obstacle" and "empty" characters, we can store them as `1`s and `0`s, dividing the memory needed by a constant factor of $8$ (bits in one `char`)!!!!
	> this is very cool because it's low level, a bit more efficient, and harder to implement, just for fun
* for the `int` data, we don't need $x\times y$, but only $x+1$, because we only need to store one line at a time  
we just need to keep track of the biggest square we found so far, and its index
```
. . . . . . . . o . . o . . . . . . . . . .
o o . . . . . o . . . . . . . . . . . . . o
. . . . . o x x x x x x . . . . . o . . . .
. . . . . . x x x x x x o . . o . . . . . .
. . o . . . x x x x x x . . . . . . . . . .
. . . . . . x x x x x x . . . o . . . . o .
. . o . . . x x x x x x o . . . . . . . . .
. . . . . . x x x x x x . . . . d[t~.~.~.~.
.~.~.~.~.~o~.~.~.~.~.~.~.~.~.~.~l]? . . . .
. . . . . . . . . . o . . . . . . . . . . .
```
> the `int` buffer holds the cells between `[` and `]`, following the `~`: the end of the previous row, and the start of the current row.  
> to find the value of `?`, we look at `l` (left), `t` (top) and `d` (top-left). `d` was already overwritten when we computed `l`, so we keep it in one extra variable, `diag`. then `?` overwrites `t`, and `t` becomes the next `diag`.

the buffer has one extra cell at the start that is always `0`: it's the column left of the map. the buffer also starts filled with `0`s: that's the row above the map. so the cells on the top and left edges need no special case, their neighbours are just `0`, so they get `0 + 1 = 1`.

this makes the peak memory footprint 
$\lceil(x\times y)/8\rceil+B+(x+1)\times 4$ `bytes`, where $B$ is the read buffer: the first power of two from $64$ that fits the first line, so less than $2\times (x+1)$ `bytes`  
instead of $((x\times y) \times 5)$ `bytes`  

measured with `valgrind --tool=massif`: on an 80 by 40 map, $400 + 128 + 324 = 852$ bytes. on a 2000 by 2000 map, $500000 + 2048 + 8004 = 510052$ bytes, and about $2000$ `read` calls (one per line) instead of 4 million when reading one byte at a time.

> why not skip the bitmap, and read the file a second time to print it? I tried: it saves the bitmap ($12.5$ MB on a 10000 by 10000 map), and the second read is almost free because the file is still in the system's cache. but the standard input can't be read twice, so `./bsq < map` still needs the bitmap, a pipe passed by name (`./bsq <(cat map)`) can't be read twice either, and it took 3 more functions than the 2 file limit allows. not worth it.

indexes are calculated like this, where `x` is the width of the map, `r` the row and `c` the column
* the index in the contiguous array is `i = r * x + c`
* the `bit` is at `arr[i / 8] & (1 << (i % 8))`
* column `c` is at `row[c + 1]` in the int buffer, `row[0]` is the always-`0` column left of the map
* the three neighbours are `row[c]` (left, already from the current row), `row[c + 1]` (top, still from the previous row) and `diag` (top-left)

# how to use

in your terminal, enter the following commands:  
```git clone git@github.com:blejjkbinks/big-Nepaleon-square.git bsq```  
`cd bsq`  
`make` (or `make spaced` for the spaced-out output, `make debug` for a build with address and undefined behaviour sanitizers)  
`./map.pl` to generate a map (80 by 40, 7% obstacles, `.ox`)  
`./map.pl [width] [height] [density%] [empty] [obstacle] [full]` for different sizes, densities or characters  
* numbers: one number is the density, two are width and height, three are width, height and density
* characters: one character is `full`, two are `empty` and `obstacle`, three are all of them
* `./map.pl -h` prints the usage

`./map.pl [options] > map` to generate a file  
`cat map` to display the file  
`./bsq map` to find THE BIGGEST SQUARE!!  
`./bsq map1 map2 ...` to solve several maps, or `./bsq < map` to read from the standard input


### norminette atrocities

the official norminette pdf reads: "A function whose only goal is to avoid the
norm, without a unique logical purpose, is also considered as a
bad function. Please remember that it is desirable to have clear and
readable functions that achieve a clear and simple task each. Avoid
any code obfuscation techniques, such as the one-liner."  
however, in practice, norminette rules have some unintended effects. here is a rough example:  

<table>
<tr>
<th>normal healthy programmer brain</th>
<th>programmer with norminette survivor syndrome</th>
</tr>
<tr>
<td>
  
```c
int	ft_isprint(char c)
{
	if (' ' <= c && c <= '~')
		return (1);
	else
		return (0);
}

int	ft_isdigit(char c)
{
	if ('0' <= c && c <= '9')
		return (1);
	else
		return (0);
}

int	ft_atoi(char *str)
{
	int	i;
	int	ret;

	i = 0;
	ret = 0;
	while (ft_isdigit(str[i]))
	{
		ret *= 10;
		ret += str[i] - '0';
		i++;
	}
	return (ret);
}

char	*get_rules(int fd, t_bsq *bsq, int *cap)
{
	char	*buffer;
	char	c;
	int		i;

	*cap = 4;
	buffer = (char *)malloc(*cap * sizeof(char));
	if (buffer == NULL)
	{
		write(2, "malloc fail\n", 12);
		return (NULL);
	}
	i = 0;
	while (read(fd, &c, 1) == 1)
	{
		buffer[i] = c;
		if (buffer[i] == '\n')
			break ;
		i++;
		if (i == *cap)
		{
			buffer = ft_realloc(buffer, cap);
		}
		buffer[i] = 0;
	}
	bsq->y = ft_atoi(buffer);
	bsq->e = buffer[i - 3];
	bsq->o = buffer[i - 2];
	bsq->f = buffer[i - 1];
	if (!ft_isprint(bsq->e) || !ft_isprint(bsq->o) || !ft_isprint(bsq->f))
	{
		free(buffer);
		return (NULL);
	}
	if (bsq->e == bsq->o || bsq->e == bsq->f || bsq->o == bsq->f)
	{
		free(buffer);
		return (NULL);
	}
	return (buffer);
}
```
  
</td>
<td>

```c


























void	*ft_free(void *ptr)
{
	free(ptr);
	return (NULL);
}

char	*get_rules(int fd, t_bsq *bsq, int *cap)
{
	char	*buffer;
	int		i;

	*cap = 4;
	buffer = (char *)malloc(*cap);
	if (!buffer && write(2, "malloc fail\n", 12) + 2)
		return (NULL);
	while (read(fd, &buffer[bsq->i], 1) == 1 && buffer[bsq->i] != '\n')
		if (++bsq->i == *cap)
			buffer = ft_realloc(buffer, cap);
	i = 0;
	while ('0' <= buffer[i] && buffer[i] <= '9' && i < bsq->i - 3)
		bsq->y = (bsq->y * 10) + buffer[i++] - '0';
	while (i++ < bsq->i)
		if (!(' ' <= buffer[i - 1] && buffer[i - 1] <= '~'))
			return (ft_free(buffer));
	bsq->e = buffer[bsq->i - 3];
	bsq->o = buffer[bsq->i - 2];
	bsq->f = buffer[bsq->i - 1];
	if (bsq->e == bsq->o || bsq->e == bsq->f || bsq->o == bsq->f)
		return (ft_free(buffer));
	return (buffer);
}

















```

</td>
</tr>
</table>

### norminette tricks used in this repo
every one of these is real, from `src/`. each one keeps a function under 25 lines, 5 variables or 4 parameters, at the cost of some readability.

| trick | what it saves | honest version |
|---|---|---|
| `bsq->err = write(2, "malloc fail\n", 12);` | `write` returns `12`, or `-1` if it fails: never `0`, so printing the message and setting the error flag is one line | `write(2, "malloc fail\n", 12);`<br>`bsq->err = 1;` |
| every reading step starts with `if (bsq->err) return ;` | `get_map` calls the steps one after the other, with no `if` around them | `if (read_rules(fd, bsq) && read_first_line(fd, bsq) && ...)`, or one `if` per step |
| `while (++c < bsq->x)` with `c = -1;` before the loop | the `c++;` line at the end of the loop body | `c = 0;`<br>`while (c < bsq->x)`<br>`{`<br>`...`<br>`c++;`<br>`}` |
| `l[0] = 0;`<br>`while (read(fd, &l[n], 1) == 1 && l[n] != '\n' && ++n < 14)`<br>`l[n] = 0;` | the `read`, the end-of-file check, the end-of-line check and the counter all live in the loop condition, so the body is one line and needs no braces. the body clears the next byte before it's read: if the file ends there, `read` writes nothing and it stays `0`, so `l[n] == '\n'` afterwards is only true when the line really ended | a `while (n < 14)` loop with a `read`, then two `if`s with a `break`, then `n++` |
| `k = (n < 14 && l[n] == '\n' && n > 3 && i == n - 3);`<br>`bsq->e = l[(n - 3) * k];` | `k` is `1` when the rules line has the right shape, `0` otherwise. multiplying by it turns a bad index into `0`, so the characters are read without going outside the array, and without an `if`/`return` | `if (n < 4 \|\| ...)`<br>`{`<br>`bsq->err = 1;`<br>`return ;`<br>`}`<br>`bsq->e = l[n - 3];` |
| `row[c + 1] = (row[c + 1] + 1) * !obstacle;` | multiplying by `0` or `1` replaces an `if`/`else`: 3 lines | `if (obstacle)`<br>`row[c + 1] = 0;`<br>`else`<br>`row[c + 1]++;` |
| the `t_bsq` struct carries the map's state everywhere | one pointer instead of up to 9 parameters (the limit is 4) and instead of 9 local variables (the limit is 5) | separate variables and parameters |
| `map[i / 8] = (map[i / 8] & ~(1 << (i % 8)))`<br>`\| (obstacle << (i % 8));` | clears the bit, then sets it if `obstacle` is `1`, in one statement and without an `if`. written inline instead of calling a `ft_bit` function: no function slot, and 200 million fewer calls on a 10000 by 10000 map (2.88s to 2.55s). `ft_bit` is still in `bsq_write.c`, commented out | `if (obstacle)`<br>`map[i / 8] \|= (1 << (i % 8));`<br>`else`<br>`map[i / 8] &= ~(1 << (i % 8));` |
