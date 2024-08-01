/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bsq.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdomange <romitdomange@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/30 00:35:20 by rdomange          #+#    #+#             */
/*   Updated: 2024/07/30 14:32:53 by rdomange         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BSQ_H
# define BSQ_H

# include <unistd.h>
# include <stdlib.h>
# include <fcntl.h>

typedef struct s_data
{
	int	y;
	int	x;
	int	e;
	int	o;
	int	f;
	int	i;
	int	j;
	int	s;
}	t_data;

void			ft_putchar(char c);
int				ft_putstr(char *str);
int				ft_strlen(char *str);
int				ft_min(int a, int b);
int				ft_atoi(char *str, int len);
char			*get_next_line(int fd);
unsigned int	*ft_mallocmap(int fd, t_data *bsq);
int				ft_bit(unsigned int *arr, int i, int val);
int				insquare(int i, int j, t_data *bsq);
void			i_love_norm(t_data *bsq, unsigned int *map, int *buffer);

#endif