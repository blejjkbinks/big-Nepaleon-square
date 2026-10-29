/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bsq.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdomange <romitdomange@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/06 16:35:35 by rdomange          #+#    #+#             */
/*   Updated: 2024/08/09 13:57:48 by rdomange         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BSQ_H
# define BSQ_H

# include <unistd.h>	//read, write
# include <stdlib.h>	//malloc, free
# include <fcntl.h>		//open, close

# define BUF_START 64	//first size of the read buffer, in bytes
# define BUF_GROW 2		//the read buffer grows by this factor

# ifndef SPACED
#  define SPACED 0		//1 prints a space between cells: make spaced
# endif

typedef struct s_bsq
{
	int		err;	//set by a step that fails, the next steps do nothing
	char	*buf;	//read buffer: the first line, then chunks of cap bytes
	long	len;	//bytes in buf
	long	cap;	//size of buf
	char	*map;	//bitmap of obstacles, x * y bits
	int		*row;	//row of square sizes, x + 1 ints, row[0] stays 0
	long	x;		//width, length of the first line
	long	y;		//height, number from the rules line
	char	e;		//empty character
	char	o;		//obstacle character
	char	f;		//full character, draws the square
	long	i;		//cells read so far, index of the next cell
	long	r;		//rows read so far
	long	b;		//index of the bottom-right cell of the biggest square
	int		s;		//size of the biggest square
	int		d;		//diag, the top-left neighbour of the current cell
}	t_bsq;

char	*get_map(int fd, t_bsq *bsq);
void	read_rules(int fd, t_bsq *bsq);
void	read_first_line(int fd, t_bsq *bsq);
void	read_map(int fd, t_bsq *bsq);

char	*ft_grow(char *src, long len, long *cap, long size);
void	solve_cell(t_bsq *bsq, int obstacle);
void	print_square(t_bsq *bsq);
void	print_line(t_bsq *bsq, char *line, long r);

#endif	//bsq.h
