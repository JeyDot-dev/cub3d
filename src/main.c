/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jordan <jordan@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/20 15:12:15 by jsousa-a          #+#    #+#             */
/*   Updated: 2023/12/28 20:00:42 by jsousa-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D.h"
// Pour les masques de mlx_hook :	1L << 0 = KeyPressMask
//									1L << 3 = ButtonRelease (pour la croix)
// Le lien avec tous les masques et events : https://harm-smits.github.io/42docs/libs/minilibx/events.html
t_vector rotate_vector(t_vector v, double angle)
{
    t_vector	result;
	float		radian; 
    float		cos_a; 
    float		sin_a;

	radian = angle * M_PI / 180.0;
	cos_a = cos(radian);
	sin_a = sin(radian);

    result.x = v.x * cos_a - v.y * sin_a;
    result.y = v.x * sin_a + v.y * cos_a;

    return result;
}

t_ray map_ray_and_dir(t_player p, t_ray r)
{
	double	camera_x;

	camera_x = 2 * r.ray_count / (double)WIN_WIDTH - 1;
	r.ray_dir.x = p.dir.x + r.cam_plane.x * camera_x;
	r.ray_dir.y = p.dir.y + r.cam_plane.y * camera_x;
	return (r);
}

t_vector calculate_delta_dist(t_ray r)
{
	t_vector	delta_dist;

	if (r.ray_dir.y == 0)
		r.ray_dir.y = 1e30;
	else
		delta_dist.y = fabs(1 / r.ray_dir.y);
	if (r.ray_dir.x == 0)
		r.ray_dir.x = 1e30;
	else
		delta_dist.x = fabs(1 / r.ray_dir.x);
	return (delta_dist);
}

t_ray calculate_initial_dist(t_player p, t_ray r)
{
	if (r.ray_dir.x < 0)
	{
		r.step.x = -1;
		r.ray_dist.x = (p.pos.x - r.map[0]) * r.delta_dist.x;
	}
	else
	{
		r.step.x = 1;
		r.ray_dist.x = (r.map[0] + 1.0 - p.pos.x) * r.delta_dist.x;
	}
	if (r.ray_dir.y < 0)
	{
		r.step.y = -1;
		r.ray_dist.y = (p.pos.y - r.map[1]) * r.delta_dist.y;
	}
	else
	{
		r.step.y = 1;
		r.ray_dist.y = (r.map[1] + 1.0 - p.pos.y) * r.delta_dist.y;
	}
	return (r);
}

t_ray digital_differential_analysis(t_ray r, char **map)
{
	while (r.hit == 0)
	{
		if (r.ray_dist.x < r.ray_dist.y)
		{
			r.ray_dist.x += r.delta_dist.x;
			r.map[0] += (int) r.step.x;
			r.side = 0;
		}
		else
		{
			r.ray_dist.y += r.delta_dist.y;
			r.map[1] += (int) r.step.y;
			r.side = 1;
		}
		if (map[r.map[1]][r.map[0]] == '1')
			r.hit = 1;
	}
	return (r);
}
void	draw_vertical_line(t_imgdata *img, int x, int y[2], int color)
{
	int	i;

	i = y[0];
	while (i < y[1])
	{
		draw_pixel(img, x, i, color);
		i++;
	}
}
void	draw_ray(t_imgdata *img, t_ray r, char **map)
{
	int	y[2];
	int	color;

	y[0] = (int) -(WIN_HEIGHT / r.perp_wall_dist) / 2 + WIN_HEIGHT / 2;
	if (y[0] < 0)
		y[0] = 0;
	y[1] = (int) (WIN_HEIGHT / r.perp_wall_dist) / 2 + WIN_HEIGHT / 2;
	if (y[1] >= WIN_HEIGHT)
		y[1] = WIN_HEIGHT - 1;
	if (map[r.map[1]][r.map[0]] == '1')
	{
		color = rgbo_color(120, 160, 70, 0);
	}
	else
		color = 0x0000FF;
	if (r.side == 1)
		color = color / 2;
	draw_vertical_line(img, r.ray_count, y, color);

}
double calculate_perp_wall_distance(t_ray r)
{
	double	perp_wall_dist;

	if (r.side == 0)
		perp_wall_dist = r.ray_dist.x - r.delta_dist.x;
	else
		perp_wall_dist = r.ray_dist.y - r.delta_dist.y;
	return (perp_wall_dist);
}

int	ray_caster(t_level level, t_imgdata *img)
{
	t_ray		ray;
	
	ray = level.ray;
	level.ray.ray_count = 0;
	while (ray.ray_count < WIN_WIDTH)
	{
		ray.hit = 0;
		printf("ray_count: %d\n", ray.ray_count);
		ray.map[0] = (int) level.player.pos.x;
		ray.map[1] = (int) level.player.pos.y;
		ray = map_ray_and_dir(level.player, ray);
		printf("ray_dir.x: %f\n", ray.ray_dir.x);
		printf("ray_dir.y: %f\n", ray.ray_dir.y);
		ray.delta_dist = calculate_delta_dist(ray);
		printf("delta_dist.x: %f\n", ray.delta_dist.x);
		printf("delta_dist.y: %f\n", ray.delta_dist.y);
		ray = calculate_initial_dist(level.player, ray);
		printf("INITIAL\n");
		printf("ray_dist.x: %f\n", ray.ray_dist.x);
		printf("ray_dist.y: %f\n", ray.ray_dist.y);
		ray = digital_differential_analysis(ray, level.map);
		printf("ray_dist.x: %f\n", ray.ray_dist.x);
		printf("ray_dist.y: %f\n", ray.ray_dist.y);
		ray.perp_wall_dist = calculate_perp_wall_distance(ray);
		draw_ray(img, ray, level.map);
		ray.ray_count++;
	}
	return (0);
}

t_level default_position(t_level level, double fov)
{
	level.player.pos.x = 9;
	level.player.pos.y = 4;
	level.player.dir.x = 0.8;
	level.player.dir.y = 0.2;
	level.player.fov = fov;
	level.ray.cam_plane.x = level.player.dir.y * tan(fov / 2 * M_PI / 180);
	level.ray.cam_plane.y = level.player.dir.x * tan(fov / 2 * M_PI / 180);
	return (level);

}

int hook_resize(void)
{
	return (0);
}
void	print_map(char **map, t_player player)
{
	int	i;
	int	j;

	i = 0;

	while (map[i])
	{
		j = 0;
		while (map[i][j])
		{
			if (i == (int)player.pos.y && j == (int)player.pos.x)
				printf("P");
			else
				printf("%c", map[i][j]);
			j++;
		}
		printf("\n");
		i++;
	}
}
int	main(int ac, char **av)
{
	t_level		level;
	t_imgdata	img;

	if (ac != 2)
		return (error("Wrong number of arguments"));
	if (ft_strncmp(av[1] + ft_strlen(av[1]) - 4, ".cub", 4) != 0)
		exit(error("A map <.cub> expected"));
	init(&level);
	parse(av[1], &level);
	init_mlx(&img);
	level = default_position(level, 90);
	print_map(level.map, level.player);
	ray_caster(level, &img);
	mlx_put_image_to_window(img.mlx, img.win, img.img, 0, 0);
	mlx_hook(img.win, 2, 1L << 0, key_hooks, &img.mlx);
	mlx_hook(img.win, 17, 1L << 3, close_cub3d, &img.mlx);
	mlx_hook(img.win, 25, 1L << 18, hook_resize, &img.mlx);
	mlx_loop(img.mlx);
	return (0);
}
