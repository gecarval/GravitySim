/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gecarval <gecarval@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/21 16:57:08 by gecarval          #+#    #+#             */
/*   Updated: 2024/10/28 09:42:50 by gecarval         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./includes/renderer.h"

int main(int argc, char **argv) {
	t_data *data;

	data = (t_data *)malloc(sizeof(t_data));
	if (!data) display_error(data, "malloc error\n");
	data->winx = WINX;
	data->winy = WINY;
	if (argc == 3) data->winx = ft_atoi(argv[1]);
	if (argc == 3) data->winy = ft_atoi(argv[2]);
	ft_printf("%s <WINDOW WIDTH> <WINDOW HEIGHT>\n", argv[0]);
	ft_init_program(data);
	create_gsim(data);
	data->anigsim = -1;
	data->click_hold = 0;
	data->show_tree = -1;
	data->show_collision_tree = -1;
	data->timing = 0;
	water_mark(data);
	mlx_mouse_hook(data->win, mouse_click, data);
	mlx_hook(data->win, DestroyNotify, 0, exit_data, data);
	mlx_key_hook(data->win, mlx_cooked, data);
	mlx_loop_hook(data->ini, mlx_anim, data);
	mlx_loop(data->ini);
	return (0);
}
