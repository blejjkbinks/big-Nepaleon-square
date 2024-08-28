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

typedef struct s_bsq
{
	long	x;
	long	y;
	char	e;
	char	o;
	char	f;
	long	i;
	int		s;
}	t_bsq;

int		ft_bit(char *arr, long i, int val);
void	*ft_free(void *ptr);

char	*ft_realloc(char *src, t_bsq *bsq, int *cap, int step);
int		neighbours(int *buffer, char *map, t_bsq *bsq, long i);
int		insquare(t_bsq bsq, long i);

char	*get_map(int fd, t_bsq *bsq);
char	*get_rules(int fd, t_bsq *bsq, int *cap);
int		ft_bsq(char *map, t_bsq *bsq);
void	print_square(char *map, t_bsq bsq);

#endif	//bsq.h
