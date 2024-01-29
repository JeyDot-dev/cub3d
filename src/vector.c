/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lebojo <lebojo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/21 17:58:02 by jordan            #+#    #+#             */
/*   Updated: 2024/01/29 19:30:25 by lebojo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D.h"

t_vector vector2D(double x, double y)
{
	t_vector res;

	res.x = x;
	res.y = y;
	return (res);
}

void set_vector2D(t_vector *vector, double x, double y)
{
	*vector = vector2D(x, y);
}