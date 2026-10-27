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

# define READ_BUF 4096	//max bytes per read() after the first line

# ifndef SPACED
#  define SPACED 0		//1 prints a space between cells: make spaced
# endif

typedef struct s_bsq
{
	long	x;		//width, length of the first line
	long	y;		//height, number from the rules line
	char	e;		//empty character
	char	o;		//obstacle character
	char	f;		//full character, draws the square
	long	i;		//cells read so far, index of the next cell
	long	r;		//rows read (or printed) so far
	long	b;		//index of the bottom-right cell of the biggest square
	int		s;		//size of the biggest square
	int		d;		//diag, the top-left neighbour of the current cell
	int		*w;		//row of square sizes, x + 1 ints, w[0] stays 0
	char	*m;		//bitmap of obstacles, NULL when the file is read again
}	t_bsq;

int		ft_bit(char *arr, long i, int val);
void	*ft_free(void *ptr);
char	*ft_grow(char *src, long len, long *cap, long size);
void	solve_cell(t_bsq *bsq, int obstacle);
int		alloc_map(t_bsq *bsq, int keep);

int		get_map(int fd, t_bsq *bsq, int keep);
int		get_rules(int fd, t_bsq *bsq);
char	*first_line(int fd, t_bsq *bsq, long *len, long *cap);
char	*fit_buf(char *buf, long len, long *cap, t_bsq *bsq);
int		parse(char *buf, long len, t_bsq *bsq);

void	print_line(t_bsq *bsq);
void	put_char(t_bsq *bsq, char ch);
void	flush_line(t_bsq *bsq, char *line);
void	reprint(t_bsq *bsq, char *path);

void	ft_print(t_bsq *bsq, char *path);

#endif	//bsq.h
