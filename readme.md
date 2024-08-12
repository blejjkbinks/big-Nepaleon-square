# BSQ
[42 C piscine](https://42.fr/en/admissions/42-piscine/) final project  
will you find the biggest square??
## problem definition
The aim of this project is to find the biggest square on a map, avoiding obstacles.  
```
10.ox
........o..o..........
oo.....o.............o
.....o...........o....
............o..o......
..o...................
...............o....o.
..o.........o.........
......................
.....o................
..........o...........
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
The first line of the map (`10.ox`) tells us the number of lines, the `empty`, `obstacle` and `square` characters.  
Invalid maps (different line lengths, invalid or duplicate characters, ...) must return "```map error```"  
<sub>My program spaces out the output to render an actual square, but the provided map generator does not. The maps we read from do not include spaces.</sub>

## constraints: limited functions and [42 norminette](https://github.com/42School/norminette/blob/master/pdf/en.norm.pdf) compliant
`open`, `close`, `read`, `write`, `malloc`, `free`. that's it!!  
max 5 functions per file, 25 lines per function, 80 columns per line  
max 4 parameters and 5 variable declaration per function  
no "for loops"
```
	% norminette 
src/bsq.h: OK!
src/get_map.c: OK!
src/norminette.c: OK!
src/bsq.c: OK!
	% norminette -v
norminette 3.3.55
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
* otherwise, smallest of its three neighbours (to its top left), + 1


### squeezed tiny memory footprint
for a map of dimensions $x\times y$,  
your first instinct might be to allocate $x\times y$ `char` to store the map and $x\times y$ `int` to calculate the values.  
but we can do much better!!:  
* because we keep track of the map's dimension in a separate `t_bsq` variable, we can store the map in a one-dimension `char` array, which means simpler allocation, saving space that would be needed for multiple pointers, and more efficient accessing.  
* when reading the map, no allocated memory is wasted:  
	* we start with `4 bytes` to read the "rules line". if more is needed, we double this array
	* this same array is then used to read the first line of the map, doubling again as many times as needed  
	* at the end of the first line, we can allocate the exact space for the whole map
	> this is really tricky because $y$ is given, but $x$ is not, until we're done with the whole first line  
* because the map is made of only "obstacle" and "empty" characters, we can store them as `1`s and `0`s, dividing the memory needed by a constant factor of $8$ (bits in one `char`)!!!!
	> this is very cool because it's low level, a bit more efficient, and harder to implement, just for fun
* for the `int` data, we don't need $x\times y$, but only $x+2$, because we only need to store one line at a time  
we just need to keep track of the biggest square we found so far, and its index
```
. .[a b~c~.~.~.~o~.~.~o~.~.~.~.~.~.~.~.~.~.
.~.~d ?]! . . o . . . . . . . . . . . . . o
. . . . . o x x x x x x . . . . . o . . . .
. . . . . . x x x x x x o . . o . . . . . .
. . o . . . x x x x x x . . . . . . . . . .
. . . . . . x x x x x x . . . o . . . . o .
. . o . . . x x x x x x o . . . . . . . . .
. . . . . . x x x x x x . . . . .[a b~c~.~.
.~.~.~.~.~o~.~.~.~.~.~.~.~.~.~.~.~d ?]! . .
. . . . . . . . . . o . . . . . . . . . . .
```
> to find the value of `?`, we look at cells `a`, `b`, and `d`. after that we can overwrite `a` to store `!`, looking at cells `b`, `c` and `?`  

this makes the total memory footprint 
$((x\times y)/8)+((x+2)\times 4)$ `bytes` ($+(x\times 4)$ to read the first line)  
instead of $((x\times y) \times 5)$ `bytes`  

indexes are calculated like this, where `x` is the width of the map and `i` the index in the contiguous array  
* the `row` position is `i / x`
* the `col` position is `i % x`
* the `buf` position in the int array is `i % (x + 2)`
* the three neighbours are `(i - 1)%(x+2)` `(i + 1)%(x+2)` and `(i + 2)%(x+2)`

# how to use

in your terminal, enter the following commands:  
```git clone git@github.com:blejjkbinks/big-Nepaleon-square.git bsq```  
`cd bsq`  
`make`  
`./map.pl` to generate a map  
`./map.pl [width] [height] [a] [b] [X]` for different map sizes or characters  
you might need to `chmod 777 map.pl`  
`./map.pl [options] > map` to generate a file  
`cat map` to display the file  
`./bsq map` to find THE BIGGEST SQUARE!!


### norminette atrocities

the official norminette pdf read: "A function whose only goal is to avoid the
norm, without a unique logical purpose, is also considered as a
bad function. Please remembder that it is desirable to have clear and
readable functions that achieve a clear and simple task each. Avoid
any code obfuscation techniques, such as the one-liner.."  
however, in practice, norminette rules have some unintended effect, here is a rough example:  

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

char *get_rules(int fd, int *cap)
{
char	*buffer;
char	c;
int		i;

*cap = 4;
buffer = (char *)malloc(*cap * sizeof(char));
if (buffer == NULL)
{
	write(1, "malloc fail\n", 12);
	return (NULL);
}
i = 0;
while (read(fd, &c, 1) == 1)
{
	buffer[i] = c;
	if (buffer[i] == '\n')
		break ;
	i++;
	if (i == cap)
	{	
		*cap = *cap * 2;
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

char *get_rules(int fd, int *cap)
{
int		i;
char	*buffer;

*cap = 4;
buffer = (char *)malloc(*cap);
if (!buffer && write(1, "malloc fail\n", 12) + 2)
	return (NULL);
while (read(fd, buffer[bsq->i], 1) == 1 && buffer[bsq->i] != '\n')
	if (bsq->i++ == *cap)
		ft_realloc(buffer, bsq, cap, 0);
i = 0;
while ('0' <= line[i] && line[i] <= '9' && i < bsq->i - 3)
	bsq->y = (bsq->y * 10) + line[i++] - '0';
while (i++ < bsq->i)
	if (!(' ' <= buffer[i - 1] && buffer[i - 1] <= '~'))
		return (ft_free(buffer));
if (bsq->e == bsq->o || bsq->e == bsq->f || bsq->o == bsq->f)
	return (ft_free(buffer));
return (buffer)
}


















```

</td>
</tr>
</table>
