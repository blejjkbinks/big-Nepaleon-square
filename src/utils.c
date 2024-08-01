/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdomange <romitdomange@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/30 00:41:47 by rdomange          #+#    #+#             */
/*   Updated: 2024/07/30 14:32:34 by rdomange         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bsq.h"

void	ft_putchar(char c)
{
	write(1, &c, 1);
}

int	ft_putstr(char *str)
{
	int	i;

	i = 0;
	while (str[i])
		ft_putchar(str[i++]);
	return (i);
}

int	ft_strlen(char *str)
{
	int	i;

	if (!str)
		return (0);
	i = 0;
	while (str[i])
		i++;
	return (i);
}

int	ft_min(int a, int b)
{
	if (a < b)
		return (a);
	return (b);
}

int	ft_atoi(char *str, int len)
{
	int	ret;
	int	i;
	int	sign;

	if (!str)
		return (0);
	i = 0;
	sign = +1;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	ret = 0;
	while ('0' <= str[i] && str[i] <= '9' && i < len)
	{
		ret *= 10;
		ret += str[i] - '0';
		i++;
	}
	return (ret * sign);
}
