/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gravitysim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anonymous <anonymous@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 12:18:49 by gecarval          #+#    #+#             */
/*   Updated: 2024/09/23 13:59:53 by gecarval         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./includes/renderer.h"

float_t	mapfloat_t(float_t value, float_t inmin, float_t inmax, float_t outmin, float_t outmax)
{
	return ((value - inmin) * (outmax - outmin) / (inmax - inmin) + outmin);
}

int	create_rgb_color(int r, int g, int b)
{
	return ((r << 16) | (g << 8) | b);
}

int	set_redshift(t_particle *tmp, t_data *data)
{
	int		redintensity;
	float_t		speedmag;

	speedmag = vector_magsq(tmp->vel);
	redintensity = (int)mapfloat_t(speedmag, 0.0f, data->num_of_particles / 500, 255.0f, 0.0f);
	if (redintensity > 255)
		redintensity = 255;
	if (redintensity < 0)
		redintensity = 0;
	return (create_rgb_color(255, redintensity, redintensity));
}

void	render_particle(t_data *data)
{
	int			i;
	int			j;
	t_particle	*tmp;

	i = -1;
	tmp = data->gsim->part;
	render_background(data, 0x000000);
	while (++i < data->num_of_particles)
	{
		tmp->color = set_redshift(tmp, data);
		if (tmp->r == 1)
		{
			pixel_to_img((int)tmp->pos.x, (int)tmp->pos.y, data, tmp->color);
		}
		else
		{
			j = tmp->r;
			while (--j >= 0)
				circlebres((int)tmp->pos.x, (int)tmp->pos.y, j, data, tmp->color);
		}
		tmp->color = WHITE;
		tmp = tmp->next;
	}
}

void	applyforce(t_particle *mover, t_vector force)
{
	t_vector	f;

	f = vectordiv(force, mover->mass);
	mover->acel = vectoradd(mover->acel, f);
}

void	attraction(t_particle *mover, t_particle *other)
{
	t_vector	force;
	float_t		strength;
	float_t		dist;
	float_t		g;

	g = 0.001;
	force = (t_vector){0, 0};
	force = vectorsub(mover->pos, other->pos);
	dist = constrain_float_t(vector_magsq(force), 100, 1000);
	strength = (g * other->mass) / dist;
	force = vector_setmagmult(force, -strength);
	applyforce(mover, force);
}

void	collision(t_particle *p1, t_particle *p2, float_t d)
{
	t_vector	impactvector;
	t_vector	deltava;
	t_vector	deltavb;
	t_vector	vdiff;
	t_vector	dir;
	float_t		overlap;
	float_t		msum;
	float_t		num;
	float_t		den;

	if (d < p1->r + p2->r)
	{
		impactvector = vectorsub(p2->pos, p1->pos);
		overlap = d - (p1->r + p2->r);
		dir = impactvector;
		dir = vector_setmagmult(dir, overlap * 0.5);
		p1->pos = vectoradd(p1->pos, dir);
		dir = vectormult(dir, -1);
		p2->pos = vectoradd(p2->pos, dir);
		d = p1->r + p2->r;
		impactvector = vector_setmagmult(impactvector, d);
		msum = p1->mass + p2->mass;
		vdiff = vectorsub(p2->vel, p1->vel);
		num = vdiff.x * impactvector.x + vdiff.y * impactvector.y;
		den = msum * d * d;
		deltava = impactvector;
		deltava = vectormult(deltava, 1 * p2->mass * num / den);
		p1->vel = vectoradd(p1->vel, deltava);
		deltavb = impactvector;
		deltavb = vectormult(deltavb, -1 * p1->mass * num / den);
		p2->vel = vectoradd(p2->vel, deltavb);
	}
}

void	limit_velocity(t_particle *mover, float_t limit)
{
	if (mover->vel.x > limit)
		mover->vel.x = limit;
	if (mover->vel.y > limit)
		mover->vel.y = limit;
	if (mover->vel.x < -limit)
		mover->vel.x = -limit;
	if (mover->vel.y < -limit)
		mover->vel.y = -limit;
}

void	update_position(t_particle *mover)
{
	mover->vel = vectoradd(mover->vel, mover->acel);
	limit_velocity(mover, 3000);
	mover->pos = vectoradd(mover->pos, mover->vel);
	mover->acel = create_vector(0, 0);
}

void	process_velocity(t_data *data)
{
	int			i;
	t_particle	*tmp;

	i = -1;
	tmp = (data->gsim->part);
	while (++i < data->num_of_particles)
	{
		tmp->prev_pos = tmp->pos;
		update_position(tmp);
		if (tmp->pos.x > data->winx || tmp->pos.x < 0)
			tmp->vel.x = -tmp->vel.x;
		if(tmp->pos.y > data->winy || tmp->pos.y < 0)
			tmp->vel.y = -tmp->vel.y;
		tmp = tmp->next;
	}
}

void	apply_attraction_onquad(t_particle *m, t_quadtree* qtree, t_data *data)
{
	t_particle	*p;
	t_particle	temp;
	t_vector	qdist;
	float_t 	d;
	int		i;

	if (qtree->divided && qtree->points == NULL)
	{
		apply_attraction_onquad(m, qtree->northeast, data);
		apply_attraction_onquad(m, qtree->northwest, data);
		apply_attraction_onquad(m, qtree->southeast, data);
		apply_attraction_onquad(m, qtree->southwest, data);
		return ;
	}
	qdist = create_vector(qtree->boundary.x, qtree->boundary.y);
	qdist = vectorsub(m->pos, qdist);
	d = vector_magsqsqrt(qdist);
	if (d < 1)
	{
		i = -1;
		while (++i < qtree->point_count)
		{
			p = qtree->points[i].part;
			if (m != p)
				attraction(m, p);
		}
	}
	else
	{
		temp.pos.x = qtree->boundary.x;
		temp.pos.y = qtree->boundary.y;
		temp.mass = m->mass * qtree->point_count;
		attraction(m, &temp);
	}
}

void	apply_collision_onquad(t_quadtree *qt, t_data *data)
{
	int		i;
	int		j;
	t_vector	dist;
	t_particle	*tmp;
	t_particle	*tmp2;

	if (qt->divided)
	{
		apply_collision_onquad(qt->northwest, data);
		apply_collision_onquad(qt->northeast, data);
		apply_collision_onquad(qt->southwest, data);
		apply_collision_onquad(qt->southeast, data);
		return ;
	}
	i = -1;
	while (++i < qt->point_count)
	{
		j = i;
		while (++j < qt->point_count)
		{
			tmp = (qt->points[i]).part;
			tmp2 = (qt->points[j]).part;
			dist = vectorsub(tmp2->pos, tmp->pos);
			collision(tmp, tmp2, vector_magsqsqrt(dist));
		}
	}
}

void	*partition_process(void *ptr)
{
	t_data		*data;
	t_particle	*tmp;
	int		i;

	i = 0;
	data = (t_data *)ptr;
	tmp = data->gsim->part;
	while (i < data->num_of_particles / 2)
	{
		tmp = tmp->next;
		i++;
	}
	while (i < data->num_of_particles)
	{
		apply_attraction_onquad(tmp, data->qt, data);
		tmp = tmp->next;
		i++;
	}
	return (NULL);
}

void	process_physics_quad(t_data *data)
{
	int		i;
	int		k;
	t_point		pt;
	t_particle	*tmp;
	pthread_t	processor;

	i = -1;
	tmp = data->gsim->part;
	data->qt = create_quadtree_fromglobals(data->winx, data->winy);
	while (++i < data->num_of_particles)
	{
		pt = create_point(tmp->pos.x, tmp->pos.y, tmp);
		insert_point(data->qt, pt);
		tmp = tmp->next;
	}
	if (pthread_create(&processor, NULL, partition_process, data) != 0)
	{
		free_quadtree(data->qt);
		display_error(data, "error on thread create\n");
	}
	i = -1;
	tmp = data->gsim->part;
	while (++i < data->num_of_particles / 2)
	{
		apply_attraction_onquad(tmp, data->qt, data);
		tmp = tmp->next;
	}
	if (pthread_join(processor, NULL) != 0)
	{
		free_quadtree(data->qt);
		display_error(data, "error on thread join\n");
	}
	if (data->show_tree == 1)
		display_quadtree_boundaries(data->qt, data);
	free_quadtree(data->qt);
	k = -1;
	while (++k < COLLISION_STEPS)
	{
		i = -1;
		tmp = data->gsim->part;
		data->qt = create_quadtree_fromvalues(data->winx / 2, data->winy / 2, data->winx, data->winy, 3);
		while (++i < data->num_of_particles)
		{
			pt = create_point(tmp->pos.x, tmp->pos.y, tmp);
			insert_point(data->qt, pt);
			tmp = tmp->next;
		}
		apply_collision_onquad(data->qt, data);
		free_quadtree(data->qt);
	}
}

void	part_sim(t_data *data)
{
	process_velocity(data);
	render_particle(data);
	process_physics_quad(data);
}
